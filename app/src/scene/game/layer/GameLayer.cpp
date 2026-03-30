#include "GameLayer.h"
#include <Features/SceneManager/SceneManager.h>
#include <drawable/particle/ParticleStorage.h>
#include <Effects/PostEffects/DepthBasedOutline/DepthBasedOutline.h>
#include <Effects/PostEffects/GaussianBloom/GaussianBloom.h>
#include <Effects/SceneTransition/TransFadeInOut.h>
#include <Effects/SceneTransition/TransShutter.h>
#include <Features/Layer/CanvasScope.h>
#include <Features/event/EventListener.h>
#include <Presets/Object3d/Grid/Preset_Grid.h>
#include <drawable/sprite/SpriteSystem.h>
#include <Math/ViewportUnits.hpp>
#include <config/ResourcePath.h>
#include <cmath>
#include <Effects/PostEffects/ScanLine/Scanline.h>
#include <Effects/PostEffects/Mosaic/Mosaic.h>
#include "Entity/Status/EntityStats.h"
#include <Presentation/ParticleType.h>
#include <logic/event/ParticleEmitEvent.h>

using namespace Math::Viewport::Unit;

void GameLayer::Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer)
{
    /// [ インスタンスの取得 ]
    pLayer_ = pLayer;
    pDx12_ = std::any_cast<DirectX12*>(pArgs->Get("DirectX12"));
    pDeltaTimeManager_ = DeltaTimeManager::GetInstance();
    randomGenerator_ = RandomGenerator::GetInstance();
    pTextureManager_ = TextureManager::GetInstance();
    pModelManager_ = std::any_cast<ModelManager*>(pArgs->Get("ModelManager"));
    pLineSystem_ = std::any_cast<LineSystem*>(pArgs->Get("LineSystem"));
    pDirectionalLight_ = std::any_cast<DirectionalLight*>(pArgs->Get("DirectionalLight"));
    pPointLight_ = std::any_cast<PointLight*>(pArgs->Get("PointLight"));
    pInputMapperUI_ = std::any_cast<InputMapper<InputActionUI>*>(pArgs->Get("InputMapperUI"));

    /// [ デバッグエントリの初期化 ]
    pDebugEntry_ = std::make_unique<DebugEntry<GameLayer>>("Scene", "GameLayer", this);

    /// [ グリッドの初期化 ]
    pGrid_ = presets::grid::Create(pModelManager_->Load("Grid_v3/Grid_v3.obj"));
    pGrid_->GetOption().lightSettingData->enablePointLight = true;
    pGrid_->GetOption().lightSettingData->enableDirectionalLight = true;
    pGrid_->SetScale(Vector3(0.5f, 30.0f, 0.5f));
    pGrid_->GetOption().materialData->tilingMultiply = Vector2(10.0f, 10.0f);

    /// [ ゲームアイの初期化 ]
    pGameEye_ = std::make_unique<GameEye>();
    pGameEye_->SetTranslate(Vector3(0, kGameEyeHeightDefault_, 0));
    pGameEye_->SetRotate(Vector3(1.57f, 0, 0));
    pGameEye_->SetName("main");

    /// [ ゲームアイをセット ]
    Object3dInstancedSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    Object3dSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    SpriteSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    LineSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    ParticleSystem::GetInstance()->SetGlobalEye(pGameEye_.get());

    /// [ 平行光源の初期化 ]
    auto& dirLightData = pDirectionalLight_->GetData();
    dirLightData.color = Vector4(0.065f, 0.058f, 0.058f, 1.0f);
    dirLightData.direction = Vector3(0.0f, -1.0f, -0.0f);
    dirLightData.intensity = 1.0f;

    /// [ ポイントライトの初期化 ]
    auto& pointLightData = pPointLight_->GetData();
    pointLightData.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    pointLightData.intensity = 7.5f;
    pointLightData.position = Vector3(0.0f, 0.0f, 2.0f);

    /// [ パーティクルの初期化 ]
    this->ParticlesInitialize();

    /// [ 移動可能範囲の設定 ]
    {
        Vector3 min = Vector3(-areaWidth_, 0.0f, -areaWidth_);
        Vector3 max = Vector3(areaWidth_, 5.0f, areaWidth_);
        playableArea_.SetMinMax(min, max);
    }

    /// [ 座標変換の初期化 ]
    screenToWorld_ = std::make_unique<ScreenToWorld>();
    screenToWorld_->Initialize();
    screenToWorld_->SetGameEye(pGameEye_.get());

    /// [ プレイヤーの初期化 ]
    Player::Params playerParams = {};
    playerParams.pModelManager = pModelManager_;
    playerParams.pDirLight = pDirectionalLight_;
    playerParams.pPointLight = pPointLight_;
    playerParams.pMovableBounds = &playableArea_;
    playerParams.pCursorPosition = &screenToWorld_->GetWorldPoint();
    pPlayer_ = std::make_unique<Player>(playerParams);
    pPlayer_->Initialize();

    /// [ プレイヤー3DUIの初期化 ]
    pPlayerUI3d_ = std::make_unique<PlayerUI3d>();
    pPlayerUI3d_->Initialize(pDx12_);

    pObject3dEnemy_ = std::make_unique<Object3dInstanced>();
    pObject3dEnemy_->Initialize();
    pObject3dEnemy_->SetModel(pModelManager_->Load("Cube/Cube.obj"));
    pObject3dEnemy_->GetOption().pMaterialData->environmentCoefficient = 0.0f;
    pObject3dEnemy_->GetOption().pLightSettingData->enableDirectionalLight = false;
    pObject3dEnemy_->GetOption().pLightSettingData->enablePointLight = false;

    /// [ 敵リポジトリの初期化 ]
    pEnemyRepository_ = std::make_unique<EnemyRepository>();

    /// [ 敵ファクトリの初期化 ]
    pEnemyFactory_ = std::make_unique<EnemyFactory>();
    EnemyContext enemyCtx = {};
    enemyCtx.pDirLight = pDirectionalLight_;
    enemyCtx.pObject3dInstanced = pObject3dEnemy_.get();
    enemyCtx.pTargetPosition = &pPlayer_->GetTransform().translate;
    pEnemyFactory_->SetContext(enemyCtx);

    
    /// [ 敵生成システムの初期化 ]
    pEnemyPopSystem_ = std::make_unique<EnemySpawner>();
    pEnemyPopSystem_->Initialize(pEnemyRepository_.get(), pEnemyFactory_.get());
    pEnemyPopSystem_->SetPopRange(Range<Vector3>(Vector3(-30.0f, 0.5f, -30.0f), Vector3(30.0f, 0.5f, 30.0f)));
    pEnemyPopSystem_->SetIgnoreRange(7.0f);

    /// [ プレイヤー弾生成システムの初期化 ]
    PlayerBulletGenerator::Config playerBulletConfig = {};
    playerBulletConfig.pParticle = particles_[static_cast<size_t>(ParticleID::Spark)];
    playerBulletConfig.numShot = 3;
    playerBulletConfig.spreadAngleDeg = 15.0f;
    playerBulletConfig.bulletSpeed = 30.0f;
    playerBulletConfig.swingSize = 0.02f;
    playerBulletGenerator_.SetConfig(playerBulletConfig);

    /// [ プレイヤー弾配列の要素を取得 ]
    playerBullets_.reserve(kMaxPlayerBullets);

    /// [ カウントダウンの初期化 ]
    pStartCountDown_ = std::make_unique<CountDown>();
    pStartCountDown_->Initialize();

    /// [ デルタタイムの設定 ]
    pDeltaTimeManager_->SetDeltaTime(0, 1.0f / 60.0f);
    pDeltaTimeManager_->SetDeltaTime(1, 1.0f / 60.0f);

    /// [ タイマー ]
    timer_.Start();

    /// [ ゲームタイマーの初期化 ]
    ingameTimer_ = std::make_unique<InGameCountDown>();
    ingameTimer_->Initialize(false, kGameLimitTime);

    /// [ 入力ガイド ]
    inputGuide_ = std::make_unique<InputGuide>();
    inputGuide_->Initialize();

    /// [ スプライトの初期化 ]
    this->SpritesInitialize();

    /// [ スコア計算機の初期化 ]
    scoreCalculator_ = std::make_unique<ScoreCalculator>();
    scoreCalculator_->Initialize();

    /// [ スロー移動ロジックの初期化 ]
    pSlomoLogic_ = std::make_unique<SlomoLogic>();

    /// [ スロー移動エフェクトコントローラーの初期化 ]
    pSlomoEffect_ = std::make_unique<SlomoEffectController>();
    pSlomoEffect_->SetConfig({});

    /// [ エミッターグループの初期化 ]
    pEmitterGroup_ = std::make_unique<ParticleEmitterGroup>();
    particleEmitSub_ = EventListener::GetInstance()->Subscribe<ParticleEmitEvent>(
            [this](const ParticleEmitEvent& e) {
        pEmitterGroup_->Emit(static_cast<uint32_t>(e.type), e.position);
    });

    this->RegisterParticleEmitters();

    /// [ ゲームオーバーアニメーションの初期化 ]
    gameOverAnimation_ = std::make_unique<GameOverAnimation>();
    gameOverAnimation_->Initialize(
        {
            .pGameEye = pGameEye_.get(),
            .pPlayer = pPlayer_.get(),
            .pPointLight = pPointLight_,
            .pParticle = particles_[static_cast<size_t>(ParticleID::PlayerDeath)]
        }
    );
    /// [ ゲームクリアアニメーションの初期化 ]
    pGameClearAnimation_ = std::make_unique<GameClearAnimation>();
    pGameClearAnimation_->Initialize(
        {
            .pGameEye = pGameEye_.get(),
            .pPlayer = pPlayer_.get(),
            .pPointLight = pPointLight_,
            .pParticle = particles_[static_cast<size_t>(ParticleID::PlayerDeath)],
            .pSpriteClear = spriteClear_.get(),
            .pSpriteSpace = spriteSpace_.get(),
            .pScoreCalculator = scoreCalculator_.get()
        }
    );
    

    /// [ イベント登録 ]
    playerExplosionSub_ = EventListener::GetInstance()->Subscribe<PlayerExplosionEvent>(
        std::bind(&GameLayer::AddPlayerExplosion, this, std::placeholders::_1)
    );
}

void GameLayer::Finalize()
{
    /// 敵の終了処理
    pEnemyRepository_->Finalize();

    pGrid_->Finalize();
    pPlayer_->Finalize();

    for (auto& bullet : playerBullets_)
    {
        bullet->Finalize();
        bullet.reset();
    }

    CollisionManager::GetInstance()->ClearCollider();
    pPlayerUI3d_->Finalize();
    pEnemyPopSystem_->Finalize();
    pStartCountDown_->Finalize();
    screenToWorld_->Finalize();
    ingameTimer_->Finalize();
    inputGuide_->Finalize();
    lines_->Finalize();
    ParticleStorage::GetInstance()->ReleaseAllParticle();
    canvasBackground_->Finalize();
    canvasGrid_->Finalize();
    canvas3dObject_->Finalize();
    canvasParticle_->Finalize();
    canvasUI_->Finalize();
    canvasUIEffected_->Finalize();
    canvasOverall_->Finalize();
    pLayer_->RemoveCanvas(canvasBackground_.get());
    pLayer_->RemoveCanvas(canvasGrid_.get());
    pLayer_->RemoveCanvas(canvas3dObject_.get());
    pLayer_->RemoveCanvas(canvasParticle_.get());
    pLayer_->RemoveCanvas(canvasUI_.get());
    pLayer_->RemoveCanvas(canvasUIEffected_.get());
    pLayer_->RemoveCanvas(canvasOverall_.get());
}

void GameLayer::Update()
{
    static constexpr float kDirectionalLightTargetIntensity = 0.25f;

    pGameEye_->Update();
    pGrid_->Update();
    screenToWorld_->Update();
    spriteClear_->Update();
    spriteSpace_->Update();
    scoreCalculator_->Update();

    /// [ ディレクショナルライトを毎フレーム目標値に近づける ]
    if (!isEnding_)
    {
        auto& dirLightData = pDirectionalLight_->GetData();
        dirLightData.intensity = std::lerp(dirLightData.intensity, kDirectionalLightTargetIntensity, 0.0125f);
    }

    /// [ プレイヤーの更新 ]
    pPlayer_->Update();

    bool isPlayerDead = !pPlayer_->IsAlive() && !gameOverAnimation_->IsPlaying();
    if (isPlayerDead) gameOverAnimation_->Play();

    bool isClear = ingameTimer_->IsEnd() && !pGameClearAnimation_->IsPlaying();
    if (isClear) pGameClearAnimation_->Play();

    if (isPlayerDead || isClear)
    {
        pEnemyRepository_->KillAll();
        pPlayer_->DisableInput();
        pPlayer_->DisableMovement();
        pEnemyPopSystem_->StopPop();
        ingameTimer_->Reset();
        ingameTimer_->SetDisplay(false);
        isEnding_ = true;
    }

    /// [ ゲームオーバーアニメーション、ゲームクリアアニメーションの更新 ]
    gameOverAnimation_->Update();
    pGameClearAnimation_->Update();

    /// [ プレイヤーのスロー更新 ]
    this->UpdateSlomo();

    /// [ 敵生成システムの更新 ]
    this->CreateEnemy();

    /// [ 敵の更新 ]
    pObject3dEnemy_->clear();
    pEnemyRepository_->Update();
    pObject3dEnemy_->Update();

    /// [ プレイヤー弾の生成 ]
    if (pPlayer_->IsShot())
    {
        this->AddPlayerBullet();
    }

    /// [ プレイヤー弾の更新 ]
    for (auto& bullet : playerBullets_)
    {
        bullet->Update();
    }

    /// [ プレイヤー弾の削除 ]
    this->RemovePlayerBullet();

    /// [ カウントダウンの更新 ]
    pStartCountDown_->Update();

    /// [ ゲーム開始時のフラッシュ演出 ]
    if (pStartCountDown_->GetState() == CountDown::State::Start && !isGameStartFlashed_)
    {
        pDirectionalLight_->GetData().intensity = kTargetDirectionalLightFlashIntensity_;
        isGameStartFlashed_ = true;
    }

    if (pStartCountDown_->IsEnd() && !pEnemyPopSystem_->IsEnablePop() && !ingameTimer_->GetNowTime() && !isEnding_)
    {
        pEnemyPopSystem_->StartPop();
        ingameTimer_->Start();
        ingameTimer_->SetDisplay(true);
    }

    /// [ ポイントライトの更新 ]
    {
        auto& position = pPointLight_->GetData().position;
        position = pPlayer_->GetTransform().translate;
        position.y = 5.0f;
    }

    /// [ タイマーの更新 ]
    if (timer_.GetNow<float>() > countDownOffset_ && !pStartCountDown_->IsStart())
    {
        pStartCountDown_->Start();
        timer_.Reset();
        timer_.Start();
    }

    /// [ ゲームタイマーの更新 ]
    ingameTimer_->Update();

    /// [ ゲームクリア後 / ゲームオーバー後のシーン遷移 ]
    if (pGameClearAnimation_->IsFinished() && !isChangingScene_ && pInputMapperUI_->IsTrigger(InputActionUI::Confirm))
    {
        SceneManager::GetInstance()->ReserveScene("TitleScene", std::make_unique<TransShutter>());
        isChangingScene_ = true;
    }
    if (gameOverAnimation_->IsFinished() && !isChangingScene_)
    {
        SceneManager::GetInstance()->ReserveScene("TitleScene", std::make_unique<TransFadeInOut>());
        isChangingScene_ = true;
    }

    /// [ インプットガイドの更新 ]
    inputGuide_->Update();

    /// [ プレイヤーUI3Dの更新 ]
    {
        PlayerUI3d::Params param = {};
        auto& stats = pPlayer_->GetStats();
        param.hp = stats.GetHp();
        param.hpMax = stats.GetMaxHp();
        param.explosionScore = pPlayer_->GetContext().Get().explosionScore;
        param.explosionScoreMax = PlayerContext::kMaxExplosionScore;
        param.slomoTime = pSlomoLogic_->GetRemainingTime();
        param.slomoTimeMax = SlomoLogic::kSlomoTimeMax_;

        pPlayerUI3d_->SetPosition(pPlayer_->GetTransform().translate);
        pPlayerUI3d_->Update(param);
    }

    UpdatePlayerExplosion();

    pRadialBeat_->Update();

    /// [ エミッターの更新 ]
    pEmitterGroup_->UpdateEmitters();
}

void GameLayer::Draw()
{
    CanvasScope backgroundCanvasScope(canvasBackground_.get());
    {
    }

    CanvasScope gridCanvasScope(canvasGrid_.get());
    {
        pGrid_->Draw1F();
    }

    CanvasScope obj3dCanvasScope(canvas3dObject_.get());
    {
        pPlayer_->Draw1F();

        /// [ 敵の描画 ]
        pEnemyRepository_->Draw1F();
        pObject3dEnemy_->Draw1F();

        for (auto& bullet : playerBullets_)
        {
            bullet->Draw1F();
        }

        pEnemyPopSystem_->DrawArea();
        for (auto& explosion : playerExplosions_)
        {
            explosion->Draw1F();
        }
    }

    CanvasScope particleCanvasScope(canvasParticle_.get());
    {
        canvasUI_->Draw1F();
        for (auto& particle : particles_)
        {
            particle->Draw1F();
        }
    }

    CanvasScope uiEffectedCanvasScope(canvasUIEffected_.get());
    {
        if (!isEnding_)
        {
            pPlayerUI3d_->Draw1F();
            scoreCalculator_->Draw1F();
        }
        pGameClearAnimation_->Draw1F();
    }

    CanvasScope uiCanvasScope(canvasUI_.get());
    if (!isEnding_)
    {
        ingameTimer_->Draw1F();
        pStartCountDown_->Draw1F();
        inputGuide_->Draw1F();
        screenToWorld_->Draw1F();
        scoreCalculator_->Draw1F();
    }
    else
    {
        spriteClear_->Draw1F();
        spriteSpace_->Draw1F();
    }

    CanvasScope overallCanvasScope(canvasOverall_.get());
    {
        canvasBackground_->Draw1F();
        canvasGrid_->Draw1F();
        canvas3dObject_->Draw1F();
        canvasParticle_->Draw1F();
        canvasUI_->Draw1F();
        canvasUIEffected_->Draw1F();
    }
}

void GameLayer::Preload(const PreloadContext& ctx, TaskExecutor& executor)
{
    pRadialBeat_ = std::make_unique<RadialBeat>();
    pLayer_ = ctx.pLayer;
    CanvasInitialize(executor, ctx.pSceneArgs);
}

void GameLayer::ImGui()
{
    #ifdef _DEBUG

    if (ImGui::CollapsingHeader("InGame"))
    {
        bool isRunning = ingameTimer_->IsRunning();
        if (ImGui::Checkbox("GameTimer", &isRunning))
        {
            if (isRunning) ingameTimer_->Start();
            else ingameTimer_->Pause();
        }
    }

    #endif // _DEBUG
}

void GameLayer::OnSceneChangeReserved()
{

}

void GameLayer::CanvasInitialize(TaskExecutor& executor, ISceneArgs* pArgs)
{
    auto pCubemapSystem = std::any_cast<CubemapSystem*>(pArgs->Get("CubemapSystem"));

    /// [ キャンバス共通パラメータ ]
    Canvas::Params commonParams = {};
    commonParams.pDx12 = std::any_cast<DirectX12*>(pArgs->Get("DirectX12"));
    commonParams.pCubemapSystem = pCubemapSystem;
    #ifdef _DEBUG
    commonParams.pImGuiManager = std::any_cast<ImGuiManager*>(pArgs->Get("ImGuiManager"));
    #endif // _DEBUG

    /// [ 背景用キャンバス ]
    auto create_background_canvas = [=]()
    {
        Canvas::Params canvasParams = commonParams;
        canvasParams.name = "Background_Canvas";
        canvasBackground_ = std::make_unique<Canvas>();
        canvasBackground_->Initialize(canvasParams);
        canvasBackground_->SetEnableManualDraw(true);

        IPostEffect* effect = nullptr;
        effect = canvasBackground_->GetPostEffectExecutor().AddEffect(PostEffectClassName::Scanline);
        {
            auto scanline = static_cast<Scanline*>(effect);
            auto& option = scanline->GetOption();
            option.opacity = 1.0f;
            option.division = 70.0f;
            option.speed = 5.0f;
            option.color0 = Vector4(0.055f, 0.055f, 0.055f, 1.000f);
            option.color1 = Vector4(0.063f, 0.063f, 0.063f, 1.000f);
            option.isOverall = 1.0f;
            scanline->Enable(true);
        }
        pLayer_->AddCanvas(canvasBackground_.get());
    };

    /// [ グリッド用キャンバス ]
    auto create_grid_canvas = [=]()
    {
        Canvas::Params canvasParams = commonParams;
        canvasParams.name = "Grid_Canvas";
        canvasGrid_ = std::make_unique<Canvas>();
        canvasGrid_->Initialize(canvasParams);
        canvasGrid_->SetEnableManualDraw(true);
        pLayer_->AddCanvas(canvasGrid_.get());
    };

    /// [ 3Dオブジェクト用キャンバス ]
    auto create_3dobject_canvas = [=]()
    {
        Canvas::Params canvasParams = commonParams;
        canvasParams.name = "3DObject_Canvas";
        canvas3dObject_ = std::make_unique<Canvas>();
        canvas3dObject_->Initialize(canvasParams);
        canvas3dObject_->SetEnableManualDraw(true);
        IPostEffect* effect = nullptr;

        effect = canvas3dObject_->GetPostEffectExecutor().AddEffect(PostEffectClassName::GaussianBloom);
        {
            auto bloom = static_cast<GaussianBloom*>(effect);
            auto& optionBloom = bloom->GetOption();
            auto& optionLuminance = bloom->GetLuminanceOutputFilter()->GetOption();
            auto& optionGaussian = bloom->GetSeparatedGaussianFilter()->GetOption();
            optionLuminance.threshold = 0.0f;
            optionGaussian.kernelSize = 21;
            optionBloom.bloomIntensity = 1.0f;
            bloom->GetSeparatedGaussianFilter()->SetSigma(27.0f);
            bloom->Enable(true);
        }
        effect = canvas3dObject_->GetPostEffectExecutor().AddEffect(PostEffectClassName::DepthBasedOutline);
        {
            auto outline = static_cast<DepthBasedOutline*>(effect);
            auto& optionOutline = outline->GetOption();
            optionOutline.weightMultiply = 1.4f;
            outline->Enable(true);
        }
        pLayer_->AddCanvas(canvas3dObject_.get());
    };

    /// [ パーティクル用キャンバス ]
    auto create_particle_canvas = [=]()
    {
        Canvas::Params canvasParams = commonParams;
        canvasParams.name = "Particle_Canvas";
        canvasParticle_ = std::make_unique<Canvas>();
        canvasParticle_->Initialize(canvasParams);
        canvasParticle_->SetEnableManualDraw(true);
        IPostEffect* effect = canvasParticle_->GetPostEffectExecutor().AddEffect(PostEffectClassName::GaussianBloom);
        auto bloom = static_cast<GaussianBloom*>(effect);
        {
            auto& optionBloom = bloom->GetOption();
            auto& optionLuminance = bloom->GetLuminanceOutputFilter()->GetOption();
            auto& optionGaussian = bloom->GetSeparatedGaussianFilter()->GetOption();
            optionLuminance.threshold = 0.0f;
            optionGaussian.kernelSize = 21;
            optionBloom.bloomIntensity = 1.0f;
            bloom->GetSeparatedGaussianFilter()->SetSigma(27.0f);
            bloom->Enable(true);
        }
        effect = canvasParticle_->GetPostEffectExecutor().AddEffect(PostEffectClassName::Mosaic);
        auto mosaic = static_cast<Mosaic*>(effect);
        {
            auto& optionMosaic = mosaic->GetOption();
            optionMosaic.power = 400.0f;
            mosaic->Enable(true);
        }

        pLayer_->AddCanvas(canvasParticle_.get());
    };

    /// [ UI用キャンバス ]
    auto create_ui_canvas = [=]()
    {
        Canvas::Params canvasParams = commonParams;
        canvasParams.name = "UI_Canvas";
        canvasUI_ = std::make_unique<Canvas>();
        canvasUI_->Initialize(canvasParams);
        canvasUI_->SetEnableManualDraw(true);
        pLayer_->AddCanvas(canvasUI_.get());
    };

    /// [ UI用キャンバス(エフェクトあり) ]
    auto create_ui_effected_canvas = [=]()
    {
        Canvas::Params canvasParams = commonParams;
        canvasParams.name = "UI_Effected_Canvas";
        canvasUIEffected_ = std::make_unique<Canvas>();
        canvasUIEffected_->Initialize(canvasParams);
        canvasUIEffected_->SetEnableManualDraw(true);
        IPostEffect* effect = nullptr;

        effect = canvasUIEffected_->GetPostEffectExecutor().AddEffect(PostEffectClassName::GaussianBloom);
        {
            auto bloom = static_cast<GaussianBloom*>(effect);
            auto& optionBloom = bloom->GetOption();
            auto& optionLuminance = bloom->GetLuminanceOutputFilter()->GetOption();
            auto& optionGaussian = bloom->GetSeparatedGaussianFilter()->GetOption();
            optionLuminance.threshold = 0.251f;
            optionGaussian.kernelSize = 21;
            optionBloom.bloomIntensity = 1.0f;
            bloom->GetSeparatedGaussianFilter()->SetSigma(27.0f);
            bloom->Enable(true);
        }

        pLayer_->AddCanvas(canvasUIEffected_.get());
    };

    /// [ 全体用キャンバス ]
    auto create_overall_canvas = [=]()
    {
        Canvas::Params canvasParams = commonParams;
        canvasParams.name = "Overall_Canvas";
        canvasOverall_ = std::make_unique<Canvas>();
        canvasOverall_->Initialize(canvasParams);
        IPostEffect* effect = nullptr;
        effect = canvasOverall_->GetPostEffectExecutor().AddEffect(PostEffectClassName::SeparatedGaussianFilter);
        auto gaussian = static_cast<SeparatedGaussianFilter*>(effect);
        {
            auto& optionGaussian = gaussian->GetOption();
            optionGaussian.kernelSize = 15;
            gaussian->SetSigma(10.0f);
            gaussian->Enable(false);
        }
        effect = canvasOverall_->GetPostEffectExecutor().AddEffect(PostEffectClassName::Grayscale);
        auto grayscale = static_cast<Grayscale*>(effect);
        {
            pOptionGrayscale_ = &grayscale->GetOption();
            pOptionGrayscale_->power = 0.0f;
            grayscale->Enable(true);
        }
        effect = canvasOverall_->GetPostEffectExecutor().AddEffect(PostEffectClassName::RadialBlur);
        effect->Enable(true);

        auto radialBlur = static_cast<RadialBlur*>(effect);
        radialBlur->SetBlurWidth(0.0f);
        pRadialBeat_->Initialize(radialBlur);
        pRadialBeat_->SetMaxWidth(0.02f);

        pLayer_->AddCanvas(canvasOverall_.get());
    };

    // キャンバスの生成をタスク化して実行
    executor.AddTask(create_background_canvas);
    executor.AddTask(create_grid_canvas);
    executor.AddTask(create_3dobject_canvas);
    executor.AddTask(create_particle_canvas);
    executor.AddTask(create_ui_canvas);
    executor.AddTask(create_ui_effected_canvas);
    executor.AddTask(create_overall_canvas);
}

void GameLayer::ParticlesInitialize()
{
    /// [ パーティクルの初期化 ]
    // SceneでParticleのDraw1Fを呼ぶ
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::PlayerConstant)] = ParticleStorage::GetInstance()->CreateParticle();
        IModel* model = pModelManager_->Load(Path::Model::kParticlePlane);
        pTextureManager_->LoadTexture(Path::Image::kParticleCircle);
        model->ChangeTexture(pTextureManager_->GetSrvHandleGPU(Path::Image::kParticleCircle));
        particle->Initialize(model);
        particle->reserve(1000);
    }
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::PlayerDeath)] = ParticleStorage::GetInstance()->CreateParticle();
        IModel* model = pModelManager_->Load(Path::Model::kParticlePlane);
        particle->Initialize(model);
        particle->reserve(500);
    }
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::Spark)] = ParticleStorage::GetInstance()->CreateParticle();
        IModel* model = pModelManager_->Load(Path::Model::kParticlePlane);
        particle->Initialize(model);
        particle->reserve(kMaxPlayerBullets);
        particle->SetEnableBillboard(true);
    }
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::Triangle)] = ParticleStorage::GetInstance()->CreateParticle();
        particle->Initialize(pModelManager_->Load("Triangle/Triangle.obj"));
        particle->reserve(500);
    }
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::Background)] = ParticleStorage::GetInstance()->CreateParticle();
        IModel* model = pModelManager_->Load(Path::Model::kParticlePlane);
        particle->Initialize(model);
        particle->reserve(1000);
    }
}

void GameLayer::SpritesInitialize()
{
    spriteClear_ = std::make_unique<Sprite>();
    pTextureManager_->LoadTexture(Path::Image::kClearText);
    spriteClear_->Initialize(Path::Image::kClearText);
    spriteClear_->SetAnchorPoint({ 0.5f, 0.5f });
    spriteClear_->SetPosition({ 25.0_vw, 35.7_vh });
    spriteClear_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

    spriteSpace_ = std::make_unique<Sprite>();
    pTextureManager_->LoadTexture(Path::Image::kTitleStartPrompt);
    spriteSpace_->Initialize(Path::Image::kTitleStartPrompt);
    spriteSpace_->SetAnchorPoint({ 0.5f, 0.5f });
    spriteSpace_->SetPosition({ 25.0_vw, 62.5_vh });
    spriteSpace_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });
}

void GameLayer::AddPlayerBullet()
{
    Input* pInput = Input::GetInstance();
    Vector3 direction = screenToWorld_->GetWorldPoint() - pPlayer_->GetTransform().translate;
    if (pInput->IsPadConnected())
    {
        auto& iAnalog = pInput->GetGamepadAnalogInput();
        direction = { iAnalog.thumbR.x, 0.0f, iAnalog.thumbR.y };
    }
    Vector3 position = pPlayer_->GetTransform().translate;
    auto bulletsGenerated = playerBulletGenerator_.Generate(position, direction);
    playerBullets_.insert(
        playerBullets_.end(),
        std::make_move_iterator(bulletsGenerated.begin()),
        std::make_move_iterator(bulletsGenerated.end())
    );
}

void GameLayer::RemovePlayerBullet()
{
    playerBullets_.erase(std::remove_if(playerBullets_.begin(), playerBullets_.end(), [&](auto& bullet)
    {
        bool shouldRemove = !bullet->IsAlive();
        if (shouldRemove)
        {
            bullet->Finalize();
        }
        return shouldRemove;
    }), playerBullets_.end());
}

void GameLayer::AddPlayerExplosion(const PlayerExplosionEvent&)
{
    PlayerExplosion::Params explosionParams = {};
    explosionParams.pDx12 = pDx12_;
    auto explosion = std::make_unique<PlayerExplosion>(explosionParams);
    explosion->Initialize(false);
    explosion->SetPosition(pPlayer_->GetTransform().translate);
    playerExplosions_.emplace_back(std::move(explosion));
    pRadialBeat_->Start(1.2f);
}

void GameLayer::UpdatePlayerExplosion()
{
    for (auto& explosion : playerExplosions_)
    {
        explosion->SetPosition(pPlayer_->GetTransform().translate);
        explosion->Update();
    }

    playerExplosions_.erase(std::remove_if(playerExplosions_.begin(), playerExplosions_.end(), [&](auto& e)
    {
        bool isFinished = !e->IsAlive();
        if (isFinished)
        {
            e->Finalize();
        }
        return isFinished;
    }), playerExplosions_.end());
}

void GameLayer::RegisterParticleEmitters()
{
    pEmitterGroup_->Register(static_cast<uint32_t>(ParticleType::EnemyNormalDeathSpark), {
        .pParticle = particles_[static_cast<size_t>(ParticleID::Spark)],
        .configPath = Path::ParticleEmitter::kEnemyNormalDeathSpark,
        .enableBillboard = true
        });
    pEmitterGroup_->Register(static_cast<uint32_t>(ParticleType::EnemyNormalDeathExplosion), {
       .pParticle = particles_[static_cast<size_t>(ParticleID::Triangle)],
       .configPath = Path::ParticleEmitter::kEnemyNormalDeathExplosion,
       .enableBillboard = true
        });
}

void GameLayer::CreateEnemy()
{
    pEnemyPopSystem_->SetIgnorePosition(pPlayer_->GetTransform().translate);
    pEnemyPopSystem_->Update();
}

void GameLayer::UpdateSlomo()
{
    constexpr float kDeltaTimeDefault = 1.0f / 60.0f;

    auto state = pSlomoLogic_->Update(pPlayer_->IsSlow(), pDeltaTimeManager_);

    float powerGrayscale = pOptionGrayscale_->power;
    if (!pGameClearAnimation_->IsPlaying())
    {
        SlomoEffectController::Context context = {};
        context.gameEyePosition = pGameEye_->GetTransform().translate;
        context.playerPosition = pPlayer_->GetTransform().translate;
        context.grayscalePower = pOptionGrayscale_->power;
        auto resultEffect = pSlomoEffect_->Update(state.isSlomoActive, context);
        pGameEye_->SetTranslate(resultEffect.gameEyePosition);
        powerGrayscale = resultEffect.grayscalePower;
    }

    Vector3 playerPos = pPlayer_->GetTransform().translate;
    Vector3 eyePos = pGameEye_->GetTransform().translate;
    if (pGameClearAnimation_->IsPlaying())
    {
        pDeltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Game, kDeltaTimeDefault);
        pDeltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Particle, kDeltaTimeDefault);
        powerGrayscale = 0.0f;
    }

    pOptionGrayscale_->power = powerGrayscale;
}
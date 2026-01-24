#include "GameLayer.h"
#include <Features/SceneManager/SceneManager.h>
#include <drawable/particle/ParticleStorage.h>
#include <Core/Window/Window.h>
#include <Effects/PostEffects/DepthBasedOutline/DepthBasedOutline.h>
#include <Effects/PostEffects/GaussianBloom/GaussianBloom.h>
#include <Effects/SceneTransition/TransFadeInOut.h>
#include <Effects/SceneTransition/TransShutter.h>
#include <Features/Layer/CanvasScope.h>
#include <Features/event/EventListener.h>
#include <Presets/Object3d/Grid/Preset_Grid.h>
#include <drawable/sprite/SpriteSystem.h>
#include <Math/ViewportUnits.hpp>
#include <imgui.h>
#include <mathExtension.h>
#include <config/ResourcePath.h>
#include <mathExtension.h>
#include <cmath>
#include <Effects/PostEffects/ScanLine/Scanline.h>
#include <Effects/PostEffects/Mosaic/Mosaic.h>

using namespace Math::Viewport;

void GameLayer::Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer)
{
    /// [ インスタンスの取得 ]
    pDx12_ = std::any_cast<DirectX12*>(pArgs->Get("DirectX12"));
    pDeltaTimeManager_ = DeltaTimeManager::GetInstance();
    randomGenerator_ = RandomGenerator::GetInstance();
    pTextureManager_ = TextureManager::GetInstance();
    pModelManager_ = std::any_cast<ModelManager*>(pArgs->Get("ModelManager"));
    pLineSystem_ = std::any_cast<LineSystem*>(pArgs->Get("LineSystem"));
    pLayer_ = pLayer;

    /// [ デバッグエントリの初期化 ]
    pDebugEntry_ = std::make_unique<DebugEntry<GameLayer>>("Scene", "GameLayer", this);

    /// [ グリッドの初期化 ]
    grid_ = presets::grid::Create(pModelManager_->Load("Grid_v3/Grid_v3.obj"));
    grid_->GetOption().lightingData->enableLighting = true;
    grid_->SetPointLight(&pointLight_);
    grid_->SetDirectionalLight(&directionalLight_);
    grid_->SetScale(Vector3(0.5f, 30.0f, 0.5f));
    grid_->GetOption().tilingData->tilingMultiply = Vector2(10.0f, 10.0f);

    /// [ ゲームアイの初期化 ]
    pGameEye_ = std::make_unique<GameEye>();
    pGameEye_->SetTranslate(Vector3(0, kGameEyeHeightDefault_, 0));
    pGameEye_->SetRotate(Vector3(1.57f, 0, 0));
    pGameEye_->SetName("main");

    /// [ ゲームアイをセット ]
    Object3dSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    SpriteSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    LineSystem::GetInstance()->SetGlobalEye(pGameEye_.get());
    ParticleSystem::GetInstance()->SetGlobalEye(pGameEye_.get());

    /// [ 平行光源の初期化 ]
    directionalLight_.color = Vector4(0.065f, 0.058f, 0.058f, 1.0f);
    directionalLight_.direction = Vector3(0.0f, -1.0f, -0.0f);
    directionalLight_.intensity = 1.0f;

    /// [ ポイントライトの初期化 ]
    pointLight_.IsEnable() = true;
    pointLight_.GetColor() = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    pointLight_.GetIntensity() = 7.5f;
    pointLight_.GetPosition() = Vector3(0.0f, 0.0f, 2.0f);

    /// [ パーティクルの初期化 ]
    this->ParticlesInitialize();

    /// [ エンティティ共通パラメータをパック ]
    entityCommonParams_.pDirLight = &directionalLight_;
    entityCommonParams_.pPointLight = &pointLight_;

    /// [ プレイヤーの初期化 ]
    Player::Params playerParams = {};
    playerParams.particle = particles_[static_cast<size_t>(ParticleID::PlayerConstant)];
    playerParams.pModelManager = pModelManager_;
    pPlayer_ = std::make_unique<Player>(playerParams);
    pPlayer_->Initialize(entityCommonParams_);

    /// [ プレイヤー3DUIの初期化 ]
    pPlayerUI3d_ = std::make_unique<PlayerUI3d>();
    pPlayerUI3d_->Initialize(pPlayer_.get(), pDx12_);

    /// [ 敵生成システムの初期化 ]
    enemyPopSystem_.Initialize();
    enemyPopSystem_.SetPopRange(Vector3(-30.0f, 0.5f, -30.0f), Vector3(30.0f, 0.5f, 30.0f));
    enemyPopSystem_.SetIgnoreRange(7.0f);

    /// [ カウントダウンの初期化 ]
    pStartCountDown_ = std::make_unique<CountDown>();
    pStartCountDown_->Initialize();

    /// [ デルタタイムの設定 ]
    pDeltaTimeManager_->SetDeltaTime(0, 1.0f / 60.0f);
    pDeltaTimeManager_->SetDeltaTime(1, 1.0f / 60.0f);

    /// [ 座標変換の初期化 ]
    screenToWorld_ = std::make_unique<ScreenToWorld>();
    screenToWorld_->Initialize();
    screenToWorld_->SetGameEye(pGameEye_.get());

    /// [ タイマー ]
    timer_.Start();

    /// [ ゲームタイマーの初期化 ]
    ingameTimer_ = std::make_unique<InGameTimer>();
    ingameTimer_->Initialize(false, kGameLimitTime);

    /// [ 入力ガイド ]
    inputGuide_ = std::make_unique<InputGuide>();
    inputGuide_->Initialize();

    /// [ 敵コンテナのサイズ予約 ]
    enemies_.reserve(kMaxEnemyCount_);

    /// [ スプライトの初期化 ]
    this->SpritesInitialize();

    /// [ スコア計算機の初期化 ]
    scoreCalculator_ = std::make_unique<ScoreCalculator>();
    scoreCalculator_->Initialize();

    /// [ BGMの初期化と再生 ]
    pBGM_ = AudioManager::GetInstance()->GetNewAudio("BGM", Path::Audio::kBgmInGame);
    pBGM_->SetVolume(0.1f);
    pBGM_->Play(true);

    /// [ ゲームオーバーアニメーションの初期化 ]
    gameOverAnimation_ = std::make_unique<GameOverAnimation>();
    gameOverAnimation_->Initialize(
        {
            .pGameEye = pGameEye_.get(),
            .pPlayer = pPlayer_.get(),
            .pPointLight = &pointLight_,
            .pParticle = particles_[static_cast<size_t>(ParticleID::PlayerDeath)]
        }
    );
    /// [ ゲームクリアアニメーションの初期化 ]
    pGameClearAnimation_ = std::make_unique<GameClearAnimation>();
    pGameClearAnimation_->Initialize(
        {
            .pGameEye = pGameEye_.get(),
            .pPlayer = pPlayer_.get(),
            .pPointLight = &pointLight_,
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
    for (auto& enemy : enemies_)
    {
        enemy->Finalize();
    }

    grid_->Finalize();
    pPlayer_->Finalize();

    for (auto& bullet : playerBullets_)
    {
        bullet->Finalize();
        bullet.reset();
    }

    CollisionManager::GetInstance()->ClearCollider();
    pBGM_->Stop();
    pPlayerUI3d_->Finalize();
    enemyPopSystem_.Finalize();
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
    grid_->Update();
    screenToWorld_->Update();
    spriteClear_->Update();
    spriteSpace_->Update();
    scoreCalculator_->Update();

    /// [ ディレクショナルライトを毎フレーム目標値に近づける ]
    if (!isEnding_)
    {
        directionalLight_.intensity = std::lerp(directionalLight_.intensity, kDirectionalLightTargetIntensity, 0.0125f);
    }

    /// [ プレイヤーの更新 ]
    pPlayer_->Update();
    pPlayerUI3d_->SetPosition(pPlayer_->GetTranslation());
    pPlayerUI3d_->Update();

    bool isPlayerDead = !pPlayer_->IsAlive() && !gameOverAnimation_->IsPlaying();
    if (isPlayerDead) gameOverAnimation_->Play();

    bool isClear = ingameTimer_->IsEnd() && !pGameClearAnimation_->IsPlaying();
    if (isClear) pGameClearAnimation_->Play();

    if (isPlayerDead || isClear)
    {
        this->KillAllEnemies();
        pPlayer_->DisableInput();
        pPlayer_->DisableMovement();
        enemyPopSystem_.StopPop();
        ingameTimer_->Reset();
        ingameTimer_->SetDisplay(false);
        isEnding_ = true;
    }

    /// [ ゲームオーバーアニメーション、ゲームクリアアニメーションの更新 ]
    gameOverAnimation_->Update();
    pGameClearAnimation_->Update();

    /// [ プレイヤーの移動範囲制限 ]
    this->LimitPlayerPosition();

    /// [ プレイヤーのスロー更新 ]
    this->PlayerSlowUpdate();

    /// [ 敵生成システムの更新 ]
    this->CreateEnemy();

    for (auto& enemy : enemies_)
    {
        enemy->Update();
    }

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

    /// [ 敵の削除 ]
    this->RemoveDeadEnemy();

    /// [ プレイヤー弾の削除 ]
    this->RemovePlayerBullet();

    /// [ カウントダウンの更新 ]
    pStartCountDown_->Update();

    /// [ ゲーム開始時のフラッシュ演出 ]
    if (pStartCountDown_->GetState() == CountDown::State::Start && !isGameStartFlashed_)
    {
        directionalLight_.intensity = kTargetDirectionalLightFlashIntensity_;
        isGameStartFlashed_ = true;
    }

    if (pStartCountDown_->IsEnd() && !enemyPopSystem_.IsEnablePop() && !ingameTimer_->GetNowTime() && !isEnding_)
    {
        enemyPopSystem_.StartPop();
        ingameTimer_->Start();
        ingameTimer_->SetDisplay(true);
    }

    /// [ ポイントライトの更新 ]
    {
        auto& position = pointLight_.GetPosition();
        position = pPlayer_->GetTranslation();
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
    if (pGameClearAnimation_->IsFinished() && !isChangingScene_ && Input::GetInstance()->TriggerKey(DIK_SPACE))
    {
        SceneManager::GetInstance()->ReserveScene("TitleScene", std::make_unique<TransShutter>());
        isChangingScene_ = true;
    }
    if (gameOverAnimation_->IsFinished() && !isChangingScene_)
    {
        SceneManager::GetInstance()->ReserveScene("TitleScene", std::make_unique<TransFadeInOut>());
        isChangingScene_ = true;
    }

    /// [ BGMのフェードアウト ]
    if (isChangingScene_)
    {
        pBGM_->SetVolume(pBGM_->GetVolume() * 0.95f);
    }

    /// [ インプットガイドの更新 ]
    inputGuide_->Update();

    UpdatePlayerExplosion();
}

void GameLayer::Draw()
{
    CanvasScope backgroundCanvasScope(canvasBackground_.get());
    {
    }

    CanvasScope gridCanvasScope(canvasGrid_.get());
    {
        grid_->Draw1F();
    }

    CanvasScope obj3dCanvasScope(canvas3dObject_.get());
    {
        pPlayer_->Draw1F();
        for (auto& enemy : enemies_)
        {
            enemy->Draw1F();
        }
        for (auto& bullet : playerBullets_)
        {
            bullet->Draw1F();
        }

        enemyPopSystem_.DrawArea();
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
    pLayer_ = ctx.pLayer;
    CanvasInitialize(executor, ctx.pSceneArgs);
}

void GameLayer::ImGui()
{
    #ifdef _DEBUG

    ImGui::SeparatorText("Collider Visualization");
    if (ImGui::Checkbox("Enemy", &isDisplayColliderEnemy_))
    {
        for (auto& enemy : enemies_)
        {
            enemy->SetIsDrawCollisionArea(isDisplayColliderEnemy_);
        }
    }

    #endif // _DEBUG
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

void GameLayer::LimitPlayerPosition()
{
    /// [ プレイヤーの移動範囲制限 ]
    Vector3 playerpos = {};
    playerpos.x = Math::clamp(pPlayer_->GetTranslation().x, -areaWidth_ + 0.5f, areaWidth_ - 0.5f);
    playerpos.y = pPlayer_->GetTranslation().y;
    playerpos.z = Math::clamp(pPlayer_->GetTranslation().z, -areaWidth_ + 0.5f, areaWidth_ - 0.5f);
    pPlayer_->SetTranslation(playerpos);
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
        auto& particle = particles_[static_cast<size_t>(ParticleID::PlayerBullet)] = ParticleStorage::GetInstance()->CreateParticle();
        IModel* model = pModelManager_->Load(Path::Model::kParticlePlane);
        particle->Initialize(model);
        particle->reserve(100);
        particle->SetEnableBillboard(true);
    }
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::EnemyDeath)] = ParticleStorage::GetInstance()->CreateParticle();
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
    constexpr int32_t kNumShots = 3;
    constexpr float kSpreadAngle = 5.0f; // degrees
    constexpr float kSpreadRad = std::numbers::pi_v<float> / (360.0f / kSpreadAngle);
    constexpr float kBulletSpeed = 30.0f;

    Vector3 direction = screenToWorld_->GetWorldPoint() - pPlayer_->GetTranslation();
    for (int32_t i = 0; i < kNumShots; ++i)
    {
        // -15°〜15°の範囲で散らす
        int32_t index = i - (kNumShots / 2);
        float angle = kSpreadRad * static_cast<float>(index);

        Vector3 newDirection = {};
        newDirection.x = direction.x * std::cosf(angle) - direction.z * std::sinf(angle);
        newDirection.y = 0.0f;
        newDirection.z = direction.x * std::sinf(angle) + direction.z * std::cosf(angle);
        newDirection = newDirection.Normalized();
        newDirection.x += randomGenerator_->Generate(-0.02f, 0.02f);
        newDirection.z += randomGenerator_->Generate(-0.02f, 0.02f);
        newDirection = newDirection.Normalized();

        auto& particle = particles_[static_cast<size_t>(ParticleID::PlayerBullet)];
        particle->emplace_back({});

        auto bullet = std::make_unique<PlayerBullet>(
            PlayerBullet::Params{ &particle->GetParticleData().back() }
        );
        bullet->Initialize(entityCommonParams_, false);
        bullet->SetTranslation(pPlayer_->GetTranslation());
        bullet->SetMoveVelocity(newDirection * kBulletSpeed);
        bullet->SetIsDrawCollisionArea(isDisplayColliderPlayerBullet_);

        playerBullets_.push_back(std::move(bullet));
    }
}

void GameLayer::RemovePlayerBullet()
{
    playerBullets_.remove_if([](const std::unique_ptr<PlayerBullet>& _bullet)
    {
        if (!_bullet->IsAlive())
        {
            _bullet->Finalize();
            return true;
        }
        return false;
    });
}

void GameLayer::RemoveDeadEnemy()
{
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [&](auto& e)
    {
        bool isDead = !e->IsAlive();
        if (isDead)
        {
            e->Finalize();
            scoreCalculator_->CountEnemyDeath();
        }
        return isDead;
    }
        ),
        enemies_.end()
    );
}

void GameLayer::KillAllEnemies()
{
    for (auto& enemy : enemies_)
    {
        enemy->Finalize();
    }
    enemies_.clear();
}

void GameLayer::AddPlayerExplosion(const PlayerExplosionEvent&)
{
    PlayerExplosion::Params explosionParams = {};
    explosionParams.pDx12 = pDx12_;
    auto explosion = std::make_unique<PlayerExplosion>(explosionParams);
    explosion->Initialize(entityCommonParams_, false);
    explosion->SetTranslation(pPlayer_->GetTranslation());
    playerExplosions_.emplace_back(std::move(explosion));
}

void GameLayer::UpdatePlayerExplosion()
{
    for (auto& explosion : playerExplosions_)
    {
        explosion->SetTranslation(pPlayer_->GetTranslation());
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

void GameLayer::CreateEnemy()
{
    enemyPopSystem_.SetIgnorePosition(pPlayer_->GetTranslation());
    enemyPopSystem_.Update();
    while (enemyPopSystem_.IsExistPopRequest())
    {
        if (enemies_.size() >= kMaxEnemyCount_)
        {
            break; // 最大数に達している場合は生成しない
        }

        auto popPoint = enemyPopSystem_.GetPopPoint();

        Enemy::Params enemyParams = {};
        enemyParams.pModelSelfBody = pModelManager_->Load("Cube/Cube.obj");
        enemyParams.pParticleTriangle = particles_[static_cast<size_t>(ParticleID::EnemyDeath)];
        enemyParams.pParticleCircle = particles_[static_cast<size_t>(ParticleID::PlayerBullet)];

        auto enemy = std::make_unique<Enemy>(enemyParams);
        enemy->Initialize(entityCommonParams_, false);
        enemy->SetTranslation(popPoint);
        enemy->SetLocationProvider(pPlayer_.get());
        enemy->SetIsDrawCollisionArea(isDisplayColliderEnemy_);
        enemies_.emplace_back(std::move(enemy));
    }

}

void GameLayer::PlayerSlowUpdate()
{
    constexpr float kGameEyeFollowRateDuringSlow = 0.1f;
    constexpr float kGameEyeFollowRateNormal = 0.1f;
    constexpr float kGrayscalePowerDuringSlow = 0.75f;
    constexpr float kGrayscaleBlendRateDuringSlow = 0.1f;
    constexpr float kDeltaTimeDefault = 1.0f / 60.0f;


    Vector3 playerPos = pPlayer_->GetTranslation();
    Vector3 eyePos = pGameEye_->GetTransform().translate;
    if (pGameClearAnimation_->IsPlaying())
    {
        pDeltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Game, kDeltaTimeDefault);
        pDeltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Particle, kDeltaTimeDefault);
        pOptionGrayscale_->power = 0.0f;
    }
    else if (pPlayer_->IsSlow())
    {
        /// [ カメラをプレイヤーに近づける ]
        auto eyeTarget = Vector3(playerPos.x, kGameEyeHeightDuringSlow_, playerPos.z);
        eyePos.Lerp(eyePos, eyeTarget, kGameEyeFollowRateDuringSlow);
        pGameEye_->SetTranslate(eyePos);

        /// [ グレースケールエフェクトの強さを変える (0<) ]
        pOptionGrayscale_->power = std::lerp(
            pOptionGrayscale_->power,
            kGrayscalePowerDuringSlow,
            kGrayscaleBlendRateDuringSlow);

        /// [ ゲームの進行速度を遅くする ]
        pDeltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Game, 1.0f / 120.0f);
        pDeltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Particle, 1.0f / 180.0f);
    }
    else
    {
        eyePos.Lerp(eyePos, Vector3(playerPos.x, kGameEyeHeightDefault_, playerPos.z), kGameEyeFollowRateDuringSlow);
        pOptionGrayscale_->power = std::lerp(pOptionGrayscale_->power, 0.0f, kGrayscaleBlendRateDuringSlow);
        pGameEye_->SetTranslate(eyePos);

        pDeltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Game, kDeltaTimeDefault);
        pDeltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Particle, kDeltaTimeDefault);
    }
}
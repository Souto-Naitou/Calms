#include "GameLayer.h"
#include <Features/SceneManager/SceneManager.h>
#include <drawable/particle/ParticleStorage.h>
#include <Core/Win32/WinSystem.h>
#include <Effects/PostEffects/DepthBasedOutline/DepthBasedOutline.h>
#include <Effects/PostEffects/GaussianBloom/GaussianBloom.h>
#include <Effects/SceneTransition/TransFadeInOut.h>
#include <Effects/SceneTransition/TransShutter.h>
#include <Features/Layer/CanvasScope.h>
#include <Presets/Object3d/Grid/Preset_Grid.h>
#include <drawable/sprite/SpriteSystem.h>
#include <imgui.h>
#include <mathExtension.h>
#include <config/ResourcePath.h>
#include <mathExtension.h>


void GameLayer::Initialize(ISceneArgs* pArgs, Layer* pLayer)
{
    /// インスタンスの取得
    deltaTimeManager_ = DeltaTimeManager::GetInstance();
    randomGenerator_ = RandomGenerator::GetInstance();
    pTextureManager_ = TextureManager::GetInstance();
    pModelManager_ = std::any_cast<ModelManager*>(pArgs->Get("ModelManager"));
    pLineSystem_ = std::any_cast<LineSystem*>(pArgs->Get("LineSystem"));
    pLayer_ = pLayer;

    /// デバッグウィンドウを登録
    pDebugEntry_ = std::make_unique<DebugEntry<GameLayer>>("Scene", "GameScene", this);

    /// キャンバスの初期化
    this->CanvasInitialize(pArgs);

    /// グリッドの初期化
    grid_ = presets::grid::Create(pModelManager_->Load("Grid_v3/Grid_v3.obj"));
    grid_->GetOption().lightingData->enableLighting = true;
    grid_->SetPointLight(&pointLight_);
    grid_->SetDirectionalLight(&directionalLight_);
    grid_->SetScale(Vector3(0.5f, 30.0f, 0.5f));
    grid_->GetOption().tilingData->tilingMultiply = Vector2(10.0f, 10.0f);

    /// ゲームアイの初期化
    gameEye_ = std::make_unique<GameEye>();
    gameEye_->SetTranslate(Vector3(0, 65.0f, 0));
    gameEye_->SetRotate(Vector3(1.57f, 0, 0));
    gameEye_->SetName("main");

    /// ゲームアイをセット
    Object3dSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    SpriteSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    LineSystem::GetInstance()->SetGlobalEye(gameEye_.get());
    ParticleSystem::GetInstance()->SetGlobalEye(gameEye_.get());

    /// 平行光源の初期化
    directionalLight_.color = Vector4(0.065f, 0.058f, 0.058f, 1.0f);
    directionalLight_.direction = Vector3(0.0f, -1.0f, -0.0f);
    directionalLight_.intensity = 1.0f;

    /// ポイントライトの初期化
    pointLight_.IsEnable() = true;
    pointLight_.GetColor() = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    pointLight_.GetIntensity() = 7.5f;
    pointLight_.GetPosition() = Vector3(0.0f, 0.0f, 2.0f);

    // パーティクルの初期化
    this->ParticlesInitialize();

    /// エンティティ共通パラメータをパック
    entityCommonParams_.pDirLight = &directionalLight_;
    entityCommonParams_.pPointLight = &pointLight_;

    /// プレイヤーの初期化
    Player::Params playerParams = {};
    playerParams.particle = particles_[static_cast<size_t>(ParticleID::PlayerConstant)];
    playerParams.pModelManager = pModelManager_;
    player_ = std::make_unique<Player>(playerParams);
    player_->Initialize(entityCommonParams_);

    /// 敵生成システムの初期化
    enemyPopSystem_.Initialize();
    enemyPopSystem_.SetPopRange(Vector3(-30.0f, 0.5f, -30.0f), Vector3(30.0f, 0.5f, 30.0f));
    enemyPopSystem_.SetIgnoreRange(3.0f);

    /// カウントダウンの初期化
    countDown_ = std::make_unique<CountDown>();
    countDown_->Initialize();

    /// デルタタイムの設定
    deltaTimeManager_->SetDeltaTime(0, 1.0f / 60.0f);
    deltaTimeManager_->SetDeltaTime(1, 1.0f / 60.0f);

    /// 座標変換の初期化
    screenToWorld_ = std::make_unique<ScreenToWorld>();
    screenToWorld_->Initialize();
    screenToWorld_->SetGameEye(gameEye_.get());

    /// タイマー
    timer_.Start();

    /// ゲームタイマーの初期化
    gameTimer_ = std::make_unique<InGameTimer>();
    gameTimer_->Initialize(false, kGameLimitTime);

    /// 入力ガイド
    inputGuide_ = std::make_unique<InputGuide>();
    inputGuide_->Initialize(canvasUI_.get());

    /// エリアの初期化
    lines_ = std::make_unique<Line>(4);
    lines_->Initialize();
    lines_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

    (*lines_)[0] = Vector3(-areaWidth_, 0.5f, -areaWidth_);
    (*lines_)[1] = Vector3(areaWidth_, 0.5f, -areaWidth_);

    (*lines_)[2] = Vector3(areaWidth_, 0.5f, -areaWidth_);
    (*lines_)[3] = Vector3(areaWidth_, 0.5f, areaWidth_);

    (*lines_)[4] = Vector3(areaWidth_, 0.5f, areaWidth_);
    (*lines_)[5] = Vector3(-areaWidth_, 0.5f, areaWidth_);

    (*lines_)[6] = Vector3(-areaWidth_, 0.5f, areaWidth_);
    (*lines_)[7] = Vector3(-areaWidth_, 0.5f, -areaWidth_);

    // 敵の予約
    enemies_.reserve(kMaxEnemyCount_);

    spriteClear_ = std::make_unique<Sprite>();
    pTextureManager_->LoadTexture(Path::Image::kClearText);
    spriteClear_->Initialize(Path::Image::kClearText);
    spriteClear_->SetAnchorPoint({ 0.5f, 0.5f });
    spriteClear_->SetPosition({ WinSystem::clientWidth / 4.0f, WinSystem::clientHeight / 2.8f });
    spriteClear_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

    spriteSpace_ = std::make_unique<Sprite>();
    pTextureManager_->LoadTexture(Path::Image::kSpaceText);
    spriteSpace_->Initialize(Path::Image::kTitleStartPrompt);
    spriteSpace_->SetAnchorPoint({ 0.5f, 0.5f });
    spriteSpace_->SetPosition({ WinSystem::clientWidth / 4.0f, WinSystem::clientHeight / 1.6f });
    spriteSpace_->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f });

    /// 体力バーの初期化
    Bar2dInitParams healthBarParams = {};
    healthBarParams.barSize = Vector2(300.0f, 10.0f);
    healthBar_ = std::make_unique<Bar2d>();
    healthBar_->Initialize(healthBarParams);
    healthBar_->SetAnchorPoint({ 0.5f, 0.5f });
    healthBar_->SetPosition(Vector2(WinSystem::clientWidth / 2.0f, WinSystem::clientHeight - WinSystem::clientHeight / 9.0f));

    titleTimer_.Start();

    /// ゲームオーバーアニメーションの初期化
    GameOverAnimation::Params goaParams = {};
    goaParams.pGameEye = gameEye_.get();
    goaParams.pPlayer = player_.get();
    goaParams.pPointLight = &pointLight_;
    goaParams.pParticle = particles_[static_cast<size_t>(ParticleID::PlayerDeath)];
    gameOverAnimation_ = std::make_unique<GameOverAnimation>();
    gameOverAnimation_->Initialize(goaParams);

    GameClearAnimation::Params gcaParams = {};
    gcaParams.pGameEye = gameEye_.get();
    gcaParams.pPlayer = player_.get();
    gcaParams.pPointLight = &pointLight_;
    gcaParams.pParticle = particles_[static_cast<size_t>(ParticleID::PlayerDeath)];
    gcaParams.pSpriteClear = spriteClear_.get();
    gcaParams.pSpriteSpace = spriteSpace_.get();
    gameClearAnimation_ = std::make_unique<GameClearAnimation>();
    gameClearAnimation_->Initialize(gcaParams);
}

void GameLayer::Finalize()
{
    for (auto& enemy : enemies_)
    {
        enemy->Finalize();
    }

    grid_->Finalize();
    player_->Finalize();

    for (auto& bullet : playerBullets_)
    {
        bullet->Finalize();
        bullet.reset();
    }

    CollisionManager::GetInstance()->ClearCollider();

    enemyPopSystem_.Finalize();
    countDown_->Finalize();
    screenToWorld_->Finalize();
    gameTimer_->Finalize();
    inputGuide_->Finalize();
    lines_->Finalize();
    ParticleStorage::GetInstance()->ReleaseAllParticle();
    canvasGrid_->Finalize();
    canvas3dObject_->Finalize();
    canvasParticle_->Finalize();
    canvasUI_->Finalize();
    canvasOverall_->Finalize();
    pLayer_->RemoveCanvas(canvasGrid_.get());
    pLayer_->RemoveCanvas(canvas3dObject_.get());
    pLayer_->RemoveCanvas(canvasParticle_.get());
    pLayer_->RemoveCanvas(canvasUI_.get());
    pLayer_->RemoveCanvas(canvasOverall_.get());
}

void GameLayer::Update()
{
    static constexpr float kDirectionalLightTargetIntensity = 0.25f;

    gameEye_->Update();
    grid_->Update();
    screenToWorld_->Update();
    spriteClear_->Update();
    spriteSpace_->Update();

    /// プレイヤーの更新
    if (!isEnding_)
    {
        directionalLight_.intensity = Math::Lerp(directionalLight_.intensity, kDirectionalLightTargetIntensity, 0.0125f);
    }
    player_->Update();

    bool isPlayerDead = !player_->IsAlive() && !gameOverAnimation_->IsPlaying();
    bool isClear = gameTimer_->IsEnd() && !gameClearAnimation_->IsPlaying();
    if (isPlayerDead) gameOverAnimation_->Play();
    if (isClear) gameClearAnimation_->Play();
    if (isPlayerDead || isClear)
    {
        this->KillAllEnemies();
        player_->DisableInput();
        player_->DisableMovement();
        enemyPopSystem_.StopPop();
        gameTimer_->Reset();
        gameTimer_->SetDisplay(false);
        isEnding_ = true;
    }

    gameOverAnimation_->Update();
    gameClearAnimation_->Update();

    /// プレイヤーの移動範囲制限
    this->LimitPlayerPosition();

    /// プレイヤーのスロー更新
    PlayerSlowUpdate();

    /// 敵生成システムの更新
    CreateEnemy();

    for (auto& enemy : enemies_)
    {
        enemy->Update();
    }

    /// プレイヤー弾の生成
    if (player_->IsShot())
    {
        CreatePlayerBullet();
    }

    /// プレイヤー弾の更新
    for (auto& bullet : playerBullets_)
    {
        bullet->Update();
    }

    /// 敵の削除
    RemoveDeadEnemy();

    /// プレイヤー弾の削除
    RemovePlayerBullet();

    /// カウントダウンの更新
    countDown_->Update();
    if (countDown_->IsEnd() && !enemyPopSystem_.IsEnablePop() && !gameTimer_->GetNowTime() && !isEnding_)
    {
        enemyPopSystem_.StartPop();
        gameTimer_->Start();
        gameTimer_->SetDisplay(true);
    }

    /// ポイントライトの更新
    {
        auto& position = pointLight_.GetPosition();
        position = player_->GetTranslation();
        position.y = 5.0f;
    }


    /// タイマーの更新
    if (timer_.GetNow<float>() > countDownOffset_ && !countDown_->IsStart())
    {
        countDown_->Start();
        timer_.Reset();
        timer_.Start();
    }

    /// ゲームタイマーの更新
    gameTimer_->Update();
    if (gameClearAnimation_->IsFinished() && !isChangingScene_ && Input::GetInstance()->TriggerKey(DIK_SPACE))
    {
        SceneManager::GetInstance()->ReserveScene("TitleScene", std::make_unique<TransShutter>());
        isChangingScene_ = true;
    }

    /// ゲームオーバー後のシーン繊維
    if (gameOverAnimation_->IsFinished() && !isChangingScene_)
    {
        SceneManager::GetInstance()->ReserveScene("TitleScene", std::make_unique<TransFadeInOut>());
        isChangingScene_ = true;
    }

    /// インプットガイドの更新
    inputGuide_->Update();

    auto playerStats = static_cast<const EntityStats*>(player_->GetStats());

    healthBar_->SetMaxValue(playerStats->GetMaxHp());
    healthBar_->SetCurrentValue(playerStats->GetHp());
    healthBar_->Update();

    /// ラインの更新
    lines_->Update();
}

void GameLayer::Draw()
{
    CanvasScope gridCanvasScope(canvasGrid_.get());
    {
        grid_->Draw1F();
    }

    CanvasScope obj3dCanvasScope(canvas3dObject_.get());
    {
        player_->Draw1F();
        for (auto& enemy : enemies_)
        {
            enemy->Draw1F();
        }
        for (auto& bullet : playerBullets_)
        {
            bullet->Draw1F();
        }

        pLineSystem_->PresentDraw();
        player_->DrawLine();
        for (auto& enemy : enemies_)
        {
            enemy->DrawLine();
        }
        for (auto& bullet : playerBullets_)
        {
            bullet->DrawLine();
        }
        enemyPopSystem_.DrawArea();
    }

    CanvasScope uiCanvasScope(canvasUI_.get());
    if (!isEnding_) 
    {
        gameTimer_->Draw1F();
        countDown_->Draw1F();
        inputGuide_->Draw1F();
        healthBar_->Draw1F();
        screenToWorld_->Draw1F();
    }
    else
    {
        spriteClear_->Draw1F();
        spriteSpace_->Draw1F();
    }

    CanvasScope particleCanvasScope(canvasParticle_.get());
    {
        canvasUI_->Draw1F();
        for (auto& particle : particles_)
        {
            particle->Draw1F();
        }
    }

    CanvasScope overallCanvasScope(canvasOverall_.get());
    {
        canvasGrid_->Draw1F();
        canvas3dObject_->Draw1F();
        canvasParticle_->Draw1F();
        canvasUI_->Draw1F();
    }
}

void GameLayer::ImGui()
{
    #ifdef _DEBUG
    ImGui::SeparatorText("Collider Debug");
    if (ImGui::Checkbox("Enemy", &isDisplayColliderEnemy_))
    {
        for (auto& enemy : enemies_)
        {
            enemy->SetIsDrawCollisionArea(isDisplayColliderEnemy_);
        }
    }

    if (ImGui::Checkbox("PlayerBullet", &isDisplayColliderPlayerBullet_))
    {
        for (auto& bullet : playerBullets_)
        {
            bullet->SetIsDrawCollisionArea(isDisplayColliderPlayerBullet_);
        }
    }

    if (ImGui::Button("CountDown"))
    {
        countDown_->Start();
    }

    if (ImGui::InputFloat("FrameRate", &framerate_, 0.1f))
    {
        deltaTimeManager_->SetDeltaTime(1, 1.0f / framerate_);
    }
    #endif
}

void GameLayer::CanvasInitialize(ISceneArgs* pArgs)
{
    auto pDx12 = std::any_cast<DirectX12*>(pArgs->Get("DirectX12"));
    auto pCubemapSystem = std::any_cast<CubemapSystem*>(pArgs->Get("CubemapSystem"));

    /// キャンバス共通パラメータ
    Canvas::Params canvasParams = {};
    canvasParams.pDx12 = pDx12;
    canvasParams.pCubemapSystem = pCubemapSystem;
    #ifdef _DEBUG
    canvasParams.pImGuiManager = std::any_cast<ImGuiManager*>(pArgs->Get("ImGuiManager"));
    #endif // _DEBUG

    /// グリッド用キャンバス
    {
        canvasParams.name = "Grid_Canvas";
        canvasGrid_ = std::make_unique<Canvas>();
        canvasGrid_->Initialize(canvasParams);
        canvasGrid_->SetEnableManualDraw(true);
        pLayer_->AddCanvas(canvasGrid_.get());
    }

    /// 3Dオブジェクト用キャンバス
    {
        canvasParams.name = "3DObject_Canvas";
        canvas3dObject_ = std::make_unique<Canvas>();
        canvas3dObject_->Initialize(canvasParams);
        canvas3dObject_->SetEnableManualDraw(true);
        IPostEffect* effect = nullptr;

        effect = canvas3dObject_->GetPostEffectExecuter().AddEffect(PostEffectClassName::GaussianBloom);
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
        effect = canvas3dObject_->GetPostEffectExecuter().AddEffect(PostEffectClassName::DepthBasedOutline);
        {
            auto outline = static_cast<DepthBasedOutline*>(effect);
            auto& optionOutline = outline->GetOption();
            optionOutline.weightMultiply = 1.4f;
            outline->Enable(true);
        }
        pLayer_->AddCanvas(canvas3dObject_.get());
    }

    /// パーティクル用キャンバス
    {
        canvasParams.name = "Particle_Canvas";
        canvasParticle_ = std::make_unique<Canvas>();
        canvasParticle_->Initialize(canvasParams);
        canvasParticle_->SetEnableManualDraw(true);
        IPostEffect* effect = canvasParticle_->GetPostEffectExecuter().AddEffect(PostEffectClassName::GaussianBloom);
        auto bloom = static_cast<GaussianBloom*>(effect);
        {
            auto& optionBloom = bloom->GetOption();
            auto& optionLuminance = bloom->GetLuminanceOutputFilter()->GetOption();
            auto& optionGaussian = bloom->GetSeparatedGaussianFilter()->GetOption();
            optionLuminance.threshold = 0.0f;
            optionGaussian.kernelSize = 21;
            optionBloom.bloomIntensity = 1.0f;
            bloom->GetSeparatedGaussianFilter()->SetSigma(27.0f);
        }
        bloom->Enable(true);
        pLayer_->AddCanvas(canvasParticle_.get());
    }

    /// UI用キャンバス
    {
        canvasParams.name = "UI_Canvas";
        canvasUI_ = std::make_unique<Canvas>();
        canvasUI_->Initialize(canvasParams);
        canvasUI_->SetEnableManualDraw(true);
        pLayer_->AddCanvas(canvasUI_.get());
    }

    /// 全体用キャンバス
    {
        canvasParams.name = "Overall_Canvas";
        canvasOverall_ = std::make_unique<Canvas>();
        canvasOverall_->Initialize(canvasParams);
        IPostEffect* effect = nullptr;
        effect = canvasOverall_->GetPostEffectExecuter().AddEffect(PostEffectClassName::SeparatedGaussianFilter);
        auto gaussian = static_cast<SeparatedGaussianFilter*>(effect);
        {
            auto& optionGaussian = gaussian->GetOption();
            optionGaussian.kernelSize = 15;
            gaussian->SetSigma(10.0f);
            gaussian->Enable(false);
        }
        pLayer_->AddCanvas(canvasOverall_.get());
    }
}

void GameLayer::LimitPlayerPosition()
{
    /// [ プレイヤーの移動範囲制限 ]
    Vector3 playerpos = {};
    playerpos.x = Math::clamp(player_->GetTranslation().x, -areaWidth_ + 0.5f, areaWidth_ - 0.5f);
    playerpos.y = player_->GetTranslation().y;
    playerpos.z = Math::clamp(player_->GetTranslation().z, -areaWidth_ + 0.5f, areaWidth_ - 0.5f);
    player_->SetTranslation(playerpos);
}

void GameLayer::ParticlesInitialize()
{
    /// [ パーティクルの初期化 ]
    // SceneでParticleのDraw1Fを呼ぶ
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::PlayerConstant)] = ParticleStorage::GetInstance()->CreateParticle();
        IModel* model = pModelManager_->Load("Particle/ParticleSpark.obj");
        pTextureManager_->LoadTexture(Path::Image::kParticleCircle);
        model->ChangeTexture(pTextureManager_->GetSrvHandleGPU(Path::Image::kParticleCircle));
        particle->Initialize(model);
        particle->reserve(1000);
    }
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::PlayerDeath)] = ParticleStorage::GetInstance()->CreateParticle();
        IModel* model = pModelManager_->Load("Particle/ParticleSpark.obj");
        particle->Initialize(model);
        particle->reserve(500);
    }
    {
        auto& particle = particles_[static_cast<size_t>(ParticleID::PlayerBullet)] = ParticleStorage::GetInstance()->CreateParticle();
        IModel* model = pModelManager_->Load("Particle/ParticleSpark.obj");
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
        IModel* model = pModelManager_->Load("Particle/ParticleSpark.obj");
        particle->Initialize(model);
        particle->reserve(1000);
    }
}

void GameLayer::CreatePlayerBullet()
{
    Vector3 direction = screenToWorld_->GetWorldPoint() - player_->GetTranslation();

    direction.y = 0.0f;
    direction = direction.Normalized();
    direction.x += randomGenerator_->Generate(-0.05f, 0.05f);
    direction.z += randomGenerator_->Generate(-0.05f, 0.05f);
    direction = direction.Normalized();

    auto& particle = particles_[static_cast<size_t>(ParticleID::PlayerBullet)];
    particle->emplace_back({});

    auto bullet = std::make_unique<PlayerBullet>(PlayerBullet::Params{ &particle->GetParticleData().back() });
    bullet->Initialize(entityCommonParams_, false);
    bullet->SetTranslation(player_->GetTranslation());
    bullet->SetMoveVelocity(direction * 15.0f);
    bullet->SetIsDrawCollisionArea(isDisplayColliderPlayerBullet_);

    playerBullets_.push_back(std::move(bullet));
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

void GameLayer::CreateEnemy()
{
    enemyPopSystem_.SetIgnorePosition(player_->GetTranslation());
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
        enemy->SetLocationProvider(player_.get());
        enemy->SetIsDrawCollisionArea(isDisplayColliderEnemy_);
        enemies_.emplace_back(std::move(enemy));
    }

}

void GameLayer::PlayerSlowUpdate()
{
    Vector3 playerpos = player_->GetTranslation();
    if (gameClearAnimation_->IsPlaying())
    {
        deltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Game, 1.0f / 60.0f);
        deltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Particle, 1.0f / 60.0f);
    }
    else if (player_->IsSlow())
    {
        Vector3 eyepos = gameEye_->GetTransform().translate;
        eyepos.Lerp(eyepos, Vector3(playerpos.x, 30.0f, playerpos.z), 0.1f);

        gameEye_->SetTranslate(eyepos);

        deltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Game, 1.0f / 120.0f);
        deltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Particle, 1.0f / 180.0f);
    }
    else
    {
        Vector3 eyepos = gameEye_->GetTransform().translate;
        eyepos.Lerp(eyepos, Vector3(playerpos.x, 50.0f, playerpos.z), 0.1f);

        gameEye_->SetTranslate(eyepos);

        deltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Game, 1.0f / 60.0f);
        deltaTimeManager_->SetDeltaTime(DeltaTimeChannelReserved::Particle, 1.0f / 60.0f);
    }
}
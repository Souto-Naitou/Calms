#include "GameOverTestScene.h"

#include <Features/SceneManager/SceneManager.h>
#include <Effects/SceneTransition/TransFadeInOut.h>
#include <MathExtension/mathExtension.h>
#include <drawable/particle/ParticleStorage.h>
#include <drawable/object3d/Object3dSystem.h>
#include <drawable/sprite/SpriteSystem.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>

#include <Vector3.h>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

#include <any>
#include <Presets/Object3d/Grid/Preset_Grid.h>
#include <config/ResourcePath.h>
#include <Core/Win32/WinSystem.h>

void GameOverTestScene::Initialize()
{
    /// インスタンスの取得
    pDebugManager_ = DebugManager::GetInstance();
    deltaTimeManager_ = DeltaTimeManager::GetInstance();
    randomGenerator_ = RandomGenerator::GetInstance();
    pTextureManager_ = TextureManager::GetInstance();
    pModelManager_ = std::any_cast<ModelManager*>(pArgs_->Get("ModelManager"));
    pLineSystem_ = std::any_cast<LineSystem*>(pArgs_->Get("LineSystem"));
    auto pDx12 = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));
    auto pCubemapSystem = std::any_cast<CubemapSystem*>(pArgs_->Get("CubemapSystem"));

    /// デバッグウィンドウを登録
    #ifdef _DEBUG
    pDebugManager_->SetComponent(name_, std::bind(&GameOverTestScene::DebugWindow, this));
    #endif // _DEBUG

    /// キャンバスの初期化
    Canvas::Params canvasParams = {};
    canvasParams.name = "GameCanvas";
    canvasParams.pDx12 = pDx12;
    canvasParams.pCubemapSystem = pCubemapSystem;
    canvas_ = std::make_unique<Canvas>();
    canvas_->Initialize(canvasParams);
    pLayer_->AddCanvas(canvas_.get());

    /// グリッドの初期化
    grid_ = presets::grid::Create(pModelManager_->Load("Grid_v3/Grid_v3.obj"));
    grid_->GetOption().lightingData->enableLighting = true;
    grid_->SetPointLight(&pointLight_);
    grid_->SetDirectionalLight(&directionalLight_);

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
    directionalLight_.intensity = 3.0f;

    /// ポイントライトの初期化
    pointLight_.IsEnable() = true;
    pointLight_.GetColor() = Vector4(0.8f, 0.7f, 0.3f, 1.0f);
    pointLight_.GetIntensity() = 7.5f;
    pointLight_.GetPosition() = Vector3(0.0f, 0.0f, 2.0f);

    /// エンティティ共通パラメータをパック
    entityCommonParams_.pDirLight = &directionalLight_;
    entityCommonParams_.pPointLight = &pointLight_;

    /// プレイヤーの初期化
    Player::Params playerParams = {};
    playerParams.pModelManager = pModelManager_;
    playerParams.particle = ParticleStorage::GetInstance()->CreateParticle();
    player_ = std::make_unique<Player>(playerParams);
    player_->Initialize(entityCommonParams_);

    /// デルタタイムの設定
    deltaTimeManager_->SetDeltaTime(0, 1.0f / 60.0f);
    deltaTimeManager_->SetDeltaTime(1, 1.0f / 60.0f);

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

    /// ゲームオーバーアニメーションの初期化
    {
        particleDeath_ = ParticleStorage::GetInstance()->CreateParticle();
        particleDeath_->Initialize(pModelManager_->Load("Particle/ParticleSpark.obj"));

        GameOverAnimationInitParams params = {};
        params.pGameEye = gameEye_.get();
        params.pPlayer = player_.get();
        params.pPointLight = &pointLight_;
        params.pParticle = particleDeath_;
        gameOverAnimation_ = std::make_unique<GameOverAnimation>();
        gameOverAnimation_->Initialize(params);
    }

    spaceSprite_ = std::make_unique<Sprite>();
    spaceSprite_->Initialize(Path::Image::kSpaceText);
    spaceSprite_->SetAnchorPoint({ 0.5f, 0.5f });
    spaceSprite_->SetPosition({ WinSystem::clientWidth / 2.0f, WinSystem::clientHeight / 3.0f });

    /// タイマー
    timer_.Start();
    titleTimer_.Start();
}

void GameOverTestScene::Finalize()
{
    gameOverAnimation_->Finalize();
    grid_->Finalize();
    player_->Finalize();

    CollisionManager::GetInstance()->ClearCollider();

    lines_->Finalize();
    ParticleStorage::GetInstance()->ReleaseAllParticle();
    canvas_->Finalize();
    pLayer_->RemoveCanvas(canvas_.get());

    #ifdef _DEBUG
    pDebugManager_->DeleteComponent(name_);
    #endif // _DEBUG
}

void GameOverTestScene::Update()
{
    gameEye_->Update();
    grid_->Update();
    spaceSprite_->Update();

    gameOverAnimation_->Update();

    /// プレイヤーの更新
    player_->Update();

    /// プレイヤーの移動範囲制限
    Vector3 playerpos = {};
    playerpos.x = Math::clamp(player_->GetTranslation().x, -areaWidth_ + 0.5f, areaWidth_ - 0.5f);
    playerpos.y = player_->GetTranslation().y;
    playerpos.z = Math::clamp(player_->GetTranslation().z, -areaWidth_ + 0.5f, areaWidth_ - 0.5f);
    player_->SetTranslation(playerpos);

    /// プレイヤーのスロー更新
    PlayerSlowUpdate();

    /// ポイントライトの更新
    {
        auto& position = pointLight_.GetPosition();
        position = player_->GetTranslation();
        position.y = 5.0f;
    }

    /// ラインの更新
    lines_->Update();

    if (Input::GetInstance()->TriggerKey(DIK_SPACE) && !gameOverAnimation_->IsPlaying())
    {
        gameOverAnimation_->Play();
    }

    if (gameOverAnimation_->IsFinished() && !isChangingScene_)
    {
        SceneManager::GetInstance()->ReserveScene("GameOverTestScene", std::make_unique<TransFadeInOut>());
        isChangingScene_ = true;
    }

    if (Input::GetInstance()->TriggerKey(DIK_RETURN) && !isChangingScene_)
    {
        SceneManager::GetInstance()->ReserveScene("GameOverTestScene", std::make_unique<TransFadeInOut>());
        isChangingScene_ = true;
    }

    //scoreSystem_->Update();
}

void GameOverTestScene::Draw()
{
    // Lineの描画
    pLineSystem_->PresentDraw();

    lines_->Draw();

    player_->Draw1F();
    grid_->Draw1F();
    spaceSprite_->Draw1F();
}

void GameOverTestScene::DrawTexts()
{
}

void GameOverTestScene::PlayerSlowUpdate()
{
    Vector3 playerpos = player_->GetTranslation();
    if (player_->IsSlow())
    {
        Vector3 eyepos = gameEye_->GetTransform().translate;
        eyepos.Lerp(eyepos, Vector3(playerpos.x, 30.0f, playerpos.z), 0.1f);

        gameEye_->SetTranslate(eyepos);

        deltaTimeManager_->SetDeltaTime(1, 1.0f / 120.0f);
    }
    else
    {
        Vector3 eyepos = gameEye_->GetTransform().translate;
        eyepos.Lerp(eyepos, Vector3(playerpos.x, 50.0f, playerpos.z), 0.1f);

        gameEye_->SetTranslate(eyepos);

        deltaTimeManager_->SetDeltaTime(1, 1.0f / 60.0f);
    }
}

void GameOverTestScene::DebugWindow()
{
    #ifdef _DEBUG
    ImGui::SeparatorText("Collider Debug");

    if (ImGui::InputFloat("FrameRate", &framerate_, 0.1f))
    {
        deltaTimeManager_->SetDeltaTime(1, 1.0f / framerate_);
    }
    #endif
}

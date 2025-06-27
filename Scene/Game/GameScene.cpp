#include "GameScene.h"

#include <Features/SceneTransition/SceneTransitionManager.h>
#include <Features/SceneTransition/TransFadeInOut.h>
#include <Features/Text/TextSystem.h>
#include <MathExtension/mathExtension.h>
#include <Features/Particle/ParticleManager.h>
#include <Features/Object3d/Object3dSystem.h>
#include <Features/Sprite/SpriteSystem.h>
#include <Timer/Timer.h>

#include <Vector3.h>

#ifdef _DEBUG
#include <imgui.h>
#endif // _DEBUG

void GameScene::Initialize()
{
    /// インスタンスの取得
    pDebugManager_    = DebugManager::GetInstance();
    deltaTimeManager_ = DeltaTimeManager::GetInstance();
    randomGenerator_  = RandomGenerator::GetInstance();

    /// デバッグウィンドウを登録
    #ifdef _DEBUG
    pDebugManager_->SetComponent(name_, std::bind(&GameScene::DebugWindow, this));
    #endif // _DEBUG


    /// テキストの色を追加
    TextSystem::GetInstance()->SetColorBrush("HarfWhite", D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.5f));


    /// グリッドの初期化
    grid_ = std::make_unique<Object3d>();
    grid_->Initialize("Grid_v3.obj");
    grid_->SetName("grid");
    grid_->SetTilingMultiply({ 100.0f, 100.0f });
    grid_->SetDirectionalLight(&directionalLight_);
    grid_->SetPointLight(&pointLight_);


    /// ゲームアイの初期化
    gameEye_ = std::make_unique<GameEye>();
    gameEye_->SetTranslate(Vector3(0, 65.0f, 0));
    gameEye_->SetRotate(Vector3(1.57f, 0, 0));
    gameEye_->SetName("main");


    /// プレイヤーの初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();
    player_->SetDIContainer(&gObjDIContainer_);


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
    pointLight_.enablePointLight = 1;
    pointLight_.color = Vector4(0.8f, 0.7f, 0.3f, 1.0f);
    pointLight_.intensity = 7.5f;
    pointLight_.position = Vector3(0.0f, 0.0f, 2.0f);


    /// DIコンテナに登録
    gObjDIContainer_.Register(&directionalLight_);
    gObjDIContainer_.Register(&pointLight_);


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
    gameTimer_ = std::make_unique<GameTimer>();
    gameTimer_->Initialize(false, 60.0);


    /// 入力ガイド
    inputGuide_ = std::make_unique<InputGuide>();
    inputGuide_->Initialize();


    /// スコアシステムの初期化
    scoreSystem_ = std::make_unique<ScoreSystem>();
    scoreSystem_->Initialize();


    /// エリアの初期化
    line_ = new Line(4);
    line_->Initialize();
    line_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

    (*line_)[0] = Vector3(-areaWidth_, 0.5f, -areaWidth_);
    (*line_)[1] = Vector3(areaWidth_, 0.5f, -areaWidth_);

    (*line_)[2] = Vector3(areaWidth_, 0.5f, -areaWidth_);
    (*line_)[3] = Vector3(areaWidth_, 0.5f, areaWidth_);

    (*line_)[4] = Vector3(areaWidth_, 0.5f, areaWidth_);
    (*line_)[5] = Vector3(-areaWidth_, 0.5f, areaWidth_);

    (*line_)[6] = Vector3(-areaWidth_, 0.5f, areaWidth_);
    (*line_)[7] = Vector3(-areaWidth_, 0.5f, -areaWidth_);

    fpsText_ = std::make_unique<Text>();
    fpsText_->Initialize();
    fpsText_->SetPosition(Vector2(10.0f, 10.0f));
    fpsText_->SetFontSize(20.0f);
    fpsText_->SetColorName("White");
    fpsText_->SetAnchorPoint(TextStandardPoint::TopLeft);
    fpsText_->SetPivot(TextStandardPoint::TopLeft);
    fpsText_->SetMaxSize(Vector2(200.0f, 100.0f));
    fpsText_->SetText("FPS: 0.0");
    fpsText_->SetName("FPS");
}

void GameScene::Finalize()
{
    for (auto& enemy : enemy_)
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

    fpsText_->Finalize();
    enemyPopSystem_.Finalize();
    countDown_->Finalize();
    screenToWorld_->Finalize();
    gameTimer_->Finalize();
    inputGuide_->Finalize();
    line_->Finalize();
    scoreSystem_->Finalize();
    ParticleManager::GetInstance()->ReleaseAllParticle();

    delete line_;

    #ifdef _DEBUG
    pDebugManager_->DeleteComponent(name_);
    #endif // _DEBUG
}


void GameScene::Update()
{
    gameEye_->Update();
    grid_->Update();
    screenToWorld_->Update();

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

    /// 敵生成システムの更新
    UpdateEnemyPopSystem();


    for (auto& enemy : enemy_)
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
    RemoveEnemy();


    /// プレイヤー弾の削除
    RemovePlayerBullet();


    /// カウントダウンの更新
    countDown_->Update();
    if (countDown_->IsEnd() && !enemyPopSystem_.IsEnablePop())
    {
        enemyPopSystem_.StartPop();
        gameTimer_->Start();
        gameTimer_->SetDisplay(true);
    }


    /// ポイントライトの更新
    pointLight_.position = player_->GetTranslation();
    pointLight_.position.y = 5.0f;


    /// タイマーの更新
    if (timer_.GetNow<float>() > countDownOffset_ && !countDown_->IsStart())
    {
        countDown_->Start();
        timer_.Reset();
        timer_.Start();
    }

    /// ゲームタイマーの更新
    gameTimer_->Update();
    if (gameTimer_->IsEnd() && !isChangingScene_)
    {
        SceneTransitionManager::GetInstance()->ChangeScene("ClearScene", std::make_unique<TransFadeInOut>());
        isChangingScene_ = true;
    }

    /// インプットガイドの更新
    inputGuide_->Update();


    /// ラインの更新
    line_->Update();

    scoreSystem_->Update();

    /// テキストの更新
    fpsText_->SetText("FPS: " + std::to_string(pDebugManager_->GetFPS()));
    fpsText_->Update();
}


void GameScene::Draw2dBackGround()
{
}


void GameScene::Draw3d()
{
    grid_->Draw();

    player_->Draw();

    for (auto& enemy : enemy_)
    {
        enemy->Draw();
    }

    for (auto& bullet : playerBullets_)
    {
        bullet->Draw();
    }

    screenToWorld_->Draw();
}

void GameScene::Draw2dMidground()
{
    gameTimer_->Draw();
}

void GameScene::Draw3dMidground()
{
}

void GameScene::DrawLine()
{
    player_->DrawLine();
    for (auto& enemy : enemy_)
    {
        enemy->DrawLine();
    }
    for (auto& bullet : playerBullets_)
    {
        bullet->DrawLine();
    }

    enemyPopSystem_.DrawArea();

    line_->Draw();
}

void GameScene::Draw2dForeground()
{
    countDown_->Draw2D();
    inputGuide_->Draw();
}

void GameScene::DrawTexts()
{
    scoreSystem_->DrawTxt();
    fpsText_->Draw();
}

void GameScene::CreatePlayerBullet()
{
    Vector3 direction = screenToWorld_->GetWorldPoint() - player_->GetTranslation();

    direction.y = 0.0f;
    direction = direction.Normalize();
    direction.x += randomGenerator_->Generate(-0.05f, 0.05f);
    direction.z += randomGenerator_->Generate(-0.05f, 0.05f);
    direction = direction.Normalize();

    auto bullet = std::make_unique<PlayerBullet>();
    bullet->Initialize(false);
    bullet->SetTranslation(player_->GetTranslation());
    bullet->SetMoveVelocity(direction * 15.0f);
    bullet->SetIsDrawCollisionArea(isDisplayColliderPlayerBullet_);
    bullet->SetDIContainer(&gObjDIContainer_);

    playerBullets_.push_back(std::move(bullet));
}


void GameScene::RemovePlayerBullet()
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


void GameScene::RemoveEnemy()
{
    enemy_.remove_if([&](const std::unique_ptr<Enemy>& _enemy)
    {
        if (!_enemy->IsAlive())
        {
            _enemy->Finalize();
            scoreSystem_->CountEnemyDeath();
            return true;
        }
        return false;
    });
}


void GameScene::UpdateEnemyPopSystem()
{
    enemyPopSystem_.SetIgnorePosition(player_->GetTranslation());
    enemyPopSystem_.Update();
    while (enemyPopSystem_.IsExistPopRequest())
    {
        if (enemy_.size() >= kMaxEnemyCount_)
        {
            break; // 最大数に達している場合は生成しない
        }

        auto popPoint = enemyPopSystem_.GetPopPoint();
        auto enemy = std::make_unique<Enemy>();
        enemy->Initialize(false);
        enemy->SetTranslation(popPoint);
        enemy->SetLocationProvider(player_.get());
        enemy->SetDIContainer(&gObjDIContainer_);
        enemy->SetIsDrawCollisionArea(isDisplayColliderEnemy_);
        enemy_.push_back(std::move(enemy));
    }
}

void GameScene::PlayerSlowUpdate()
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

void GameScene::DebugWindow()
{
    #ifdef _DEBUG
    ImGui::SeparatorText("Collider Debug");
    if (ImGui::Checkbox("Enemy", &isDisplayColliderEnemy_))
    {
        for (auto& enemy : enemy_)
        {
            enemy->SetIsDrawCollisionArea(isDisplayColliderEnemy_);
        }
    }

    if (ImGui::Checkbox("Player", &isDisplayColliderPlayer_))
    {
        player_->SetIsDrawCollisionArea(isDisplayColliderPlayer_);
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

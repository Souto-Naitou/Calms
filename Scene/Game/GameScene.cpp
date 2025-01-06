#include "GameScene.h"

#include <Features/Model/ModelManager.h>

#include <Vector3.h>

#include <imgui.h>

void GameScene::Initialize()
{
    /// インスタンスの取得
    pDebugManager_ = DebugManager::GetInstance();
    

    /// デバッグウィンドウを登録
    pDebugManager_->SetComponent(name_, std::bind(&GameScene::DebugWindow, this));


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
    grid_->SetGameEye(gameEye_.get());
    player_->SetGameEye(gameEye_.get());


    /// 平行光源の初期化
    directionalLight_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    directionalLight_.direction = Vector3(0.50f, -0.28f, -0.82f);
    directionalLight_.intensity = 0.03f;

    /// ポイントライトの初期化
    pointLight_.enablePointLight = 0;
    pointLight_.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    pointLight_.intensity = 1.0f;
    pointLight_.position = Vector3(0.0f, 0.0f, 2.0f);


    /// DIコンテナに登録
    gObjDIContainer_.Register(&directionalLight_);
    gObjDIContainer_.Register(&pointLight_);


    /// 敵生成システムの初期化
    enemyPopSystem_.Initialize();
    enemyPopSystem_.SetPopInterval(2.0f);
    enemyPopSystem_.SetPopCount(2);
    enemyPopSystem_.SetPopRange(Vector3(-30.0f, 0.5f, -30.0f), Vector3(30.0f, 0.5f, 30.0f));
    enemyPopSystem_.SetIgnoreRange(2.0f);
    enemyPopSystem_.SetGameEye(gameEye_.get());


    /// カウントダウンの初期化
    countDown_ = std::make_unique<CountDown>();
    countDown_->Initialize();
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
    }

    enemyPopSystem_.Finalize();
    countDown_->Finalize();

    /// 解放を明示しないと何故かリークする
    enemy_.clear();
    grid_.reset();
    gameEye_.reset();
    player_.reset();
    playerBullets_.clear();
    countDown_.reset();

    pDebugManager_->DeleteComponent(name_.c_str());
}


void GameScene::Update()
{
    gameEye_->Update();
    grid_->Update();
    player_->Update();


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

    /// プレイヤー弾の削除
    RemovePlayerBullet();


    /// 敵の削除
    RemoveEnemy();


    /// カウントダウンの更新
    countDown_->Update();
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
}

void GameScene::Draw2dForeground()
{
    countDown_->Draw2D();
}

void GameScene::CreatePlayerBullet()
{
    auto bullet = std::make_unique<PlayerBullet>();
    bullet->Initialize();
    bullet->SetTranslation(player_->GetTranslation());
    bullet->SetMoveVelocity(Vector3(0, 0, 15.0f));
    bullet->SetGameEye(gameEye_.get());
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
    enemy_.remove_if([](const std::unique_ptr<Enemy>& _enemy)
    {
        if (!_enemy->IsAlive())
        {
            _enemy->Finalize();
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
        auto popPoint = enemyPopSystem_.GetPopPoint();
        auto enemy = std::make_unique<Enemy>();
        enemy->Initialize();
        enemy->SetTranslation(popPoint);
        enemy->SetLocationProvider(player_.get());
        enemy->SetDIContainer(&gObjDIContainer_);
        enemy->SetGameEye(gameEye_.get());
        enemy->SetIsDrawCollisionArea(isDisplayColliderEnemy_);
        enemy_.push_back(std::move(enemy));
    }
}

void GameScene::DebugWindow()
{
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
}

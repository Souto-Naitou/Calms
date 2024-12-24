#include "GameScene.h"

#include <Features/Model/ModelManager.h>

#include <Vector3.h>

void GameScene::Initialize()
{
    /// グリッドの初期化
    grid_ = std::make_unique<Object3d>();
    grid_->Initialize("Grid_v3.obj");
    grid_->SetName("grid");
    grid_->SetTilingMultiply({ 100.0f, 100.0f });
    ModelManager::GetInstance()->SetLightingFlag("Grid_v3.obj", false);


    // ゲームアイの初期化
    gameEye_ = std::make_unique<GameEye>();
    gameEye_->SetTranslate(Vector3(0, 65.0f, 0));
    gameEye_->SetRotate(Vector3(1.57f, 0, 0));
    gameEye_->SetName("main");


    /// プレイヤーの初期化
    player_ = std::make_unique<Player>();
    player_->Initialize();


    /// ゲームアイをセット
    grid_->SetGameEye(gameEye_.get());
    player_->SetGameEye(gameEye_.get());
}

void GameScene::Finalize()
{
    grid_->Finalize();
    player_->Finalize();

    for (auto& bullet : playerBullets_)
    {
        bullet->Finalize();
    }
    
    /// 解放を明示しないと何故かリークする
    grid_.reset();
    gameEye_.reset();
    player_.reset();
    playerBullets_.clear();
}

void GameScene::Update()
{
    gameEye_->Update();
    grid_->Update();
    player_->Update();

    if (player_->IsShot())
    {
        CreatePlayerBullet();
    }

    for (auto& bullet : playerBullets_)
    {
        bullet->Update();
    }

    RemovePlayerBullet();
}

void GameScene::Draw2dBackGround()
{
}

void GameScene::Draw3d()
{
    grid_->Draw();
    player_->Draw();

    for (auto& bullet : playerBullets_)
    {
        bullet->Draw();
    }
}

void GameScene::Draw2dForeground()
{
}

void GameScene::CreatePlayerBullet()
{
    auto bullet = std::make_unique<PlayerBullet>();
    bullet->Initialize();
    bullet->SetTranslation(player_->GetTranslation());
    bullet->SetMoveVelocity(Vector3(0, 0, 15.0f));
    bullet->SetGameEye(gameEye_.get());
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

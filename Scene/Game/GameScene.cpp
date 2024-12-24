#include "GameScene.h"

#include <Features/Model/ModelManager.h>

#include <Vector3.h>

void GameScene::Initialize()
{
    /// グリッドの初期化
    grid_ = std::make_unique<Object3d>();
    grid_->Initialize("Grid_v3.obj");
    grid_->SetTilingMultiply({ 100.0f, 100.0f });

    // ゲームアイの初期化
    gameEye_ = std::make_unique<GameEye>();
    gameEye_->SetTranslate(Vector3(0, 40.0f, 0));
    gameEye_->SetRotate(Vector3(1.57f, 0, 0));
    gameEye_->SetName("main");

    /// ゲームアイをセット
    grid_->SetGameEye(gameEye_.get());
    grid_->SetName("grid");
    ModelManager::GetInstance()->SetLightingFlag("Grid_v3.obj", false);
}

void GameScene::Finalize()
{
    grid_->Finalize();
    
    /// 解放を明示しないと何故かリークする
    grid_.reset();
    gameEye_.reset();
}

void GameScene::Update()
{
    gameEye_->Update();
    grid_->Update();
}

void GameScene::Draw2dBackGround()
{
}

void GameScene::Draw3d()
{
    grid_->Draw();
}

void GameScene::Draw2dForeground()
{
}

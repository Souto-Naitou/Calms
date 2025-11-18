#include "GameScene.h"


void GameScene::Initialize()
{
    pGameLayer_ = std::make_unique<GameLayer>();
    pGameLayer_->Initialize(pArgs_, pLayer_);
}

void GameScene::Finalize()
{
    pGameLayer_->Finalize();
}

void GameScene::Update()
{
    pGameLayer_->Update();
}

void GameScene::Draw()
{
    pGameLayer_->Draw();
}

void GameScene::DrawTexts()
{
}

void GameScene::ImGui()
{
}

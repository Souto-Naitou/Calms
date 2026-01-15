#include "GameScene.h"


void GameScene::Initialize()
{
    if (!pGameLayer_) pGameLayer_ = std::make_unique<GameLayer>();
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

void GameScene::PreLoad(TaskExecutor& executor)
{
    PreloadContext ctx{};
    ctx.pSceneArgs = pArgs_;
    ctx.pLayer = pLayer_;
    pGameLayer_ = std::make_unique<GameLayer>();
    pGameLayer_->Preload(ctx, executor);
}

void GameScene::ImGui()
{
}

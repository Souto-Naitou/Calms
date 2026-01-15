#pragma once

// engine
#include <Scene/SceneBase.h>
#include <Core/DirectX12/TextureManager.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/RandomGenerator/RandomGenerator.h>
#include <Features/Model/ModelManager.h>
#include <memory>
#include "layer/GameLayer.h"

/// <summary>
/// 繧ｲ繝ｼ繝繧ｷ繝ｼ繝ｳ
/// </summary>
class GameScene : public ILoadableScene
{
public:
    GameScene(ISceneArgs* pArgs) : ILoadableScene(pArgs) {};

    /// <summary>
    /// 蛻晄悄蛹・    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 邨ゆｺ・    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 譖ｴ譁ｰ
    /// </summary>
    void Update() override;


    /// <summary>
    /// 謠冗判
    /// </summary>
    void Draw() override;

    /// <summary>
    /// 繝ｭ繝ｼ繝峨す繝ｼ繝ｳ縺ｧ螳溯｡後☆繧句・逅・    /// </summary>
    void PreLoad(TaskExecutor& executor) override;


    /// <summary>
    /// 繧ｷ繝ｼ繝ｳ逕ｨ縺ｮ繝・ヰ繝・げ繧ｦ繧｣繝ｳ繝峨え繧呈緒逕ｻ縺励∪縺吶・    /// </summary>
    void ImGui();

private:
    std::unique_ptr<GameLayer> pGameLayer_ = nullptr;

private:
    DeltaTimeManager* deltaTimeManager_ = nullptr;
    RandomGenerator* randomGenerator_ = nullptr;
    ModelManager* pModelManager_ = nullptr;
    LineSystem* pLineSystem_ = nullptr;
    TextureManager* pTextureManager_ = nullptr;
};

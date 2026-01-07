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
/// ゲームシーン
/// </summary>
class GameScene : public ILoadableScene
{
public:
    GameScene(ISceneArgs* pArgs) : ILoadableScene(pArgs) {};

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;


    /// <summary>
    /// 描画
    /// </summary>
    void Draw() override;

    /// <summary>
    /// ロードシーンで実行する処理
    /// </summary>
    void PreLoad(TaskExecutor& executor) override;


    /// <summary>
    /// シーン用のデバッグウィンドウを描画します。
    /// </summary>
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
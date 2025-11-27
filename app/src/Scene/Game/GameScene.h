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
class GameScene : public SceneBase
{
public:
    GameScene(ISceneArgs* _args) : SceneBase(_args) {};

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
    /// 描画(テキスト描画を除く)
    /// </summary>
    void Draw() override;


    /// <summary>
    /// テキスト描画
    /// </summary>
    void DrawTexts() override;

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
#pragma once

// engine
#include <scene/SceneBase.h>
#include <Core/DirectX12/TextureManager.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/RandomGenerator/RandomGenerator.h>
#include <Features/Model/ModelManager.h>
#include "layer/GameLayer.h"
#include "layer/PauseLayer.h"
#include <Features/Input/Input.h>
#include <logic/event/PauseMenuToggleEvent.h>
#include <Features/event/EventSubscription.h>
#include <Effects/PostEffects/SeparatedGaussianFilter/SeparatedGaussianFilter.h>
#include <optional>
#include <memory>

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


    void OnSceneChangeReserved() override;

private:
    void TogglePauseMenu(const PauseMenuToggleEvent& nonused = {});
    void PauseBlurUpdate();

    static constexpr inline float kPauseBlurSigmaLerpFactorIncrease_ = 0.05f;
    static constexpr inline float kPauseBlurSigmaLerpFactorDecrease_ = 0.2f;
    static constexpr inline float kPauseBlurSigmaMax_ = 30.0f;

    std::unique_ptr<GameLayer>  pGameLayer_         = nullptr;
    std::unique_ptr<PauseLayer> pPauseLayer_        = nullptr;
    Audio*                      pBGM_               = nullptr;  // !< BGMポインタ
    bool                        isPauseMenuActive_  = false;
    bool                        isChangingScene_    = false;
    SeparatedGaussianFilter*    pGaussianFilter_    = nullptr;  // !< ガウシアンフィルタ (ポーズ用)

    std::optional<EventSubscription> subscriptionPauseMenuToggle_ = std::nullopt;

    Input* pInput_ = nullptr;
    DeltaTimeManager* deltaTimeManager_ = nullptr;
    RandomGenerator* randomGenerator_ = nullptr;
    ModelManager* pModelManager_ = nullptr;
    LineSystem* pLineSystem_ = nullptr;
    TextureManager* pTextureManager_ = nullptr;
};
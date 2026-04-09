#pragma once

#include <Features/Input/Input.h>
#include <Features/screen/IntermediateScreenBase.h>
#include <Core/DirectX12/TextureManager.h>
#include <drawable/sprite/Sprite.h>
#include <Features/Bar2d/Bar2d.h>
#include <Features/Layer/Canvas.h>
#include <Features/Model/ModelManager.h>
#include <string>
#include <memory>
#include <Interfaces/ISceneArgs.h>

/// <summary>
/// ロードシーン
/// </summary>
class LoadingScreen : public IntermediateScreenBase
{
public:
    LoadingScreen(ISceneArgs* args) : IntermediateScreenBase(args) {};

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
    /// Screenを終了可能かどうかを取得します。
    /// </summary>
    bool IsEnd() const override;


private:
    /// <summary>
    /// 指定ディレクトリ以下のテクスチャパスを収集します。
    /// </summary>
    /// <param name="directoryPath">探索するルートディレクトリのパス。</param>
    void AggregateTexturePaths(const std::string& directoryPath);

    void InitializeDrawables();
    void InitializeCanvas(DirectX12*, CubemapSystem*);

    Input*              pInput_             = nullptr;  // !< 入力
    TextureManager*     pTextureManager_    = nullptr;  // !< テクスチャマネージャー
    ModelManager*       pModelManager_      = nullptr;  // !< モデルマネージャー

    const float     kSmoothFactor_ = 0.1f;
    const float     kWaitTime_ = 1.0f;
    TimeMeasurer    waitTimer_ = {};
    
    std::unique_ptr<Canvas>     pCanvasScanline_            = nullptr; // !< 背景キャンバス
    std::unique_ptr<Canvas>     pCanvas_                    = nullptr; // !< ロードシーン用キャンバス
    bool                        isTexturePathAggregated_    = false;
    bool                        isChangingScene_            = false; // !< シーン遷移中かどうか

    float                       current_            = 0.0f;
    std::unique_ptr<Sprite>     pSpriteLoading_     = nullptr; // !< ローディングスプライト
    std::unique_ptr<Sprite>     pSpriteLBackground_ = nullptr; // !< ローディングスプライト背景
    std::unique_ptr<Bar2d>      pBar_               = nullptr; // !< ローディングバー
};

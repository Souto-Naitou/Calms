#pragma once

#include <Scene/SceneBase.h>
#include <Core/DirectX12/TextureManager.h>
#include <drawable/sprite/Sprite.h>
#include <Features/Bar2d/Bar2d.h>
#include <Features/Layer/Canvas.h>
#include <string>
#include <vector>
#include <memory>

/// <summary>
/// ロードシーン
/// </summary>
class LoadScene : public SceneBase
{
public:
    LoadScene(ISceneArgs* _pArg) : SceneBase(_pArg) {};

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
    void DrawTexts() override {};


private:
    Input*          pInput_             = nullptr;  // !< 入力
    TextureManager* pTextureManager_    = nullptr;  // !< テクスチャマネージャー

    const float     kSmoothFactor_ = 0.1f;
    const float     kWaitTime_ = 2.0f;
    TimeMeasurer    waitTimer_ = {};

    std::vector<std::string>    texturePaths_       = {};
    bool                        isAggregated_       = false;
    bool                        isChangingScene_    = false; // !< シーン遷移中かどうか

    float                       current_            = 0.0f;
    std::unique_ptr<Sprite>     pSpriteLoading_     = nullptr; // !< ローディングスプライト
    std::unique_ptr<Sprite>     pSpriteLBackground_ = nullptr; // !< ローディングスプライト背景
    std::unique_ptr<Bar2d>      pBar_               = nullptr; // !< ローディングバー

    std::unique_ptr<Canvas>     pCanvas_            = nullptr; // !< ロードシーン用キャンバス

    /// <summary>
    /// 指定ディレクトリ以下のテクスチャパスを収集します。
    /// </summary>
    /// <param name="directoryPath">探索するルートディレクトリのパス。</param>
    void AggregateTexturePaths(const std::string& directoryPath);
};

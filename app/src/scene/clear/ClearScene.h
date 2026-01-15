#pragma once

#include <scene/SceneBase.h>
#include <drawable/sprite/Sprite.h>
#include <memory>
#include <Features/Layer/Canvas.h>

/// <summary>
/// クリアシーン
/// </summary>
class ClearScene : public SceneBase
{
public:
    ClearScene(ISceneArgs* _pArgs) : SceneBase(_pArgs) {};

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

private:
    std::unique_ptr<Sprite> pClear_ = nullptr;
    std::unique_ptr<Sprite> pSpace_ = nullptr;
    std::unique_ptr<Canvas> canvasUI_ = nullptr;
};
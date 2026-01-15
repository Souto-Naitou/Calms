#pragma once

#include <Scene/SceneBase.h>
#include <drawable/sprite/Sprite.h>
#include <memory>
#include <Features/Layer/Canvas.h>

/// <summary>
/// 繧ｯ繝ｪ繧｢繧ｷ繝ｼ繝ｳ
/// </summary>
class ClearScene : public SceneBase
{
public:
    ClearScene(ISceneArgs* _pArgs) : SceneBase(_pArgs) {};

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

private:
    std::unique_ptr<Sprite> pClear_ = nullptr;
    std::unique_ptr<Sprite> pSpace_ = nullptr;
    std::unique_ptr<Canvas> canvasUI_ = nullptr;
};

#pragma once

#include <Scene/SceneBase.h>
#include <Features/Sprite/Sprite.h>
#include <memory>

/// <summary>
/// クリアシーン
/// </summary>
class ClearScene : public SceneBase
{
public:
    ClearScene(ISceneArgs* _pArgs) : SceneBase(_pArgs) {};

    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw() override;

    /// <summary>
    /// テキスト描画
    /// </summary>
    void DrawTexts() override;

private:
    std::unique_ptr<Sprite> pClear_ = nullptr;
    std::unique_ptr<Sprite> pSpace_ = nullptr;
};
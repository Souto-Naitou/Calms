#pragma once

#include <Interfaces/IScene.h>
#include <Features/Sprite/Sprite.h>

class ClearScene : public IScene
{
    void Initialize() override;
    void Finalize() override;
    void Update() override;
    void Draw2dBackGround() override;
    void Draw3d() override;
    void Draw2dMidground() override;
    void Draw3dMidground() override;
    void DrawLine() override;
    void Draw2dForeground() override;

    /// <summary>
    /// テキスト描画
    /// </summary>
    void DrawTexts() override;

private:
    Sprite* pClear_ = nullptr;
    Sprite* pSpace_ = nullptr;
};
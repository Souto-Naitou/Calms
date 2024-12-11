#pragma once

#include <Interfaces/IScene.h>

class TitleScene : public IScene
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    virtual void Initialize() override;

    /// <summary>
    /// 終了
    /// </summary>
    virtual void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    virtual void Update() override;

    /// <summary>
    /// 背景描画
    /// </summary>
    virtual void Draw2dBackGround() override;

    /// <summary>
    /// 3D描画
    /// </summary>
    virtual void Draw3d() override;

    /// <summary>
    /// 前景描画
    /// </summary>
    virtual void Draw2dForeground() override;


};
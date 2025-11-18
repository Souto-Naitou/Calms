#pragma once
#include <Interfaces/ISceneArgs.h>
#include <Features/Layer/Layer.h>

class IGameLayer
{
public:
    virtual ~IGameLayer() = default;

    /// <summary>
    /// 初期化を行います。
    /// </summary>
    virtual void Initialize(ISceneArgs* pArgs, Layer* pLayer) = 0;

    /// <summary>
    /// 終了処理を行います。
    /// </summary>
    virtual void Finalize() = 0;

    /// <summary>
    /// 状態更新を行います。
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// 描画処理を行います。
    /// </summary>
    virtual void Draw() = 0;
};
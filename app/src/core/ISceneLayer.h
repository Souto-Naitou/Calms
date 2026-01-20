#pragma once
#include <Interfaces/ISceneArgs.h>
#include <Features/Layer/OrderedCanvasLayer.h>
#include <io/loader/TaskExecutor.h>

/// <summary>
/// シーン層基底クラス
/// - シーン内で階層表現をするために使用します。
/// </summary>
class ISceneLayer
{
public:
    virtual ~ISceneLayer() = default;

    /// <summary>
    /// 初期化を行います。
    /// </summary>
    virtual void Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer) = 0;

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

struct PreloadContext
{
    ISceneArgs*         pSceneArgs      = nullptr;
    OrderedCanvasLayer* pLayer          = nullptr;
};

class ILoadableGameLayer : public ISceneLayer
{
public:
    virtual ~ILoadableGameLayer() = default;

    /// <summary>
    /// ロードスクリーンで実行する処理を登録します。
    /// - SceneManager::ScenePreloadを経由して、SceneTransitionExecutorで呼び出します。
    /// </summary>
    virtual void Preload(const PreloadContext& ctx, TaskExecutor& executor) = 0;
};
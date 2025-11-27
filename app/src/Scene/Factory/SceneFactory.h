#pragma once

#include <Interfaces/ISceneFactory.h>
#include <Scene/SceneBase.h>

#include <memory>
#include <functional>
#include <map>

/// <summary>
/// シーンファクトリークラス
/// </summary>
class SceneFactory : public ISceneFactory
{
public:
    SceneFactory();

    /// <summary>
    /// シーン生成
    /// </summary>
    /// <param name="_sceneName">シーン名</param>
    /// <returns>生成したシーン</returns>
    std::unique_ptr<SceneBase> CreateScene(const std::string& sceneName, ISceneArgs* pArgs) override;

private:
    std::map<std::string, std::function<std::unique_ptr<SceneBase>(ISceneArgs*)>> sceneCreators_;
};
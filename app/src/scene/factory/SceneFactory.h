#pragma once

#include <Interfaces/ISceneFactory.h>
#include <Scene/SceneBase.h>

#include <memory>
#include <functional>
#include <map>

/// <summary>
/// 繧ｷ繝ｼ繝ｳ繝輔ぃ繧ｯ繝医Μ繝ｼ繧ｯ繝ｩ繧ｹ
/// </summary>
class SceneFactory : public ISceneFactory
{
public:
    SceneFactory();

    /// <summary>
    /// 繧ｷ繝ｼ繝ｳ逕滓・
    /// </summary>
    /// <param name="_sceneName">繧ｷ繝ｼ繝ｳ蜷・/param>
    /// <returns>逕滓・縺励◆繧ｷ繝ｼ繝ｳ</returns>
    std::unique_ptr<SceneBase> Create(const std::string& sceneName, ISceneArgs* pArgs) override;


    std::unique_ptr<ILoadableScene> CreateLoadable(const std::string& sceneName, ISceneArgs* pArgs) override;

private:
    void OutputSceneMissingError(const std::string& sceneName);

    std::map<std::string, std::function<std::unique_ptr<SceneBase>(ISceneArgs*)>> sceneCreators_;
    std::map<std::string, std::function<std::unique_ptr<ILoadableScene>(ISceneArgs*)>> loadableSceneCreators_;
};

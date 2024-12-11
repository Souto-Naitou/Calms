#pragma once

#include <Interfaces/ISceneFactory.h>
#include <Interfaces/IScene.h>

class SceneFactory : public ISceneFactory
{
public:
    SceneFactory() = default;

    /// <summary>
    /// シーン生成
    /// </summary>
    /// <param name="_sceneName">シーン名</param>
    /// <returns>生成したシーン</returns>
    IScene* CreateScene(const std::string& _sceneName) override;
};
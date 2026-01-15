#pragma once

#include <Interfaces/ISceneFactory.h>
#include <scene/SceneBase.h>
#include <Interfaces/ISceneArgs.h>

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
    std::unique_ptr<SceneBase> Create(const std::string& sceneName, ISceneArgs* pArgs) override;
    std::unique_ptr<ILoadableScene> CreateLoadable(const std::string& sceneName, ISceneArgs* pArgs) override;

private:
    void OutputSceneMissingError(const std::string& sceneName);

    // シーン生成関数マップ
    // キー：シーン名、値：シーン生成関数
    // 生成関数 { 戻り値 : シーンポインタ, 引数 : シーン引数ポインタ }
    std::map<std::string, std::function<std::unique_ptr<SceneBase>(ISceneArgs*)>> sceneCreators_;
    std::map<std::string, std::function<std::unique_ptr<ILoadableScene>(ISceneArgs*)>> loadableSceneCreators_;
};
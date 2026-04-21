#include "SceneFactory.h"

#include <scene/title/TitleScene.h>
#include <scene/game/GameScene.h>
#include <scene/clear/ClearScene.h>
#include <scene/edit/EditScene.h>
#include <Utility/ConvertString/ConvertString.h>

#include <cassert>
#include <format>
#include <DebugTools/Logger/Logger.h>
#include <cstdlib>

#define JUDGE_SCENE(class, arg) if (_sceneName == #class) { return std::make_unique<class>(arg); }

SceneFactory::SceneFactory()
{
    // シーンの登録
    sceneCreators_["TitleScene"]        = [](ISceneArgs* args) { return std::make_unique<TitleScene>(args); };
    sceneCreators_["ClearScene"]        = [](ISceneArgs* args) { return std::make_unique<ClearScene>(args); };
    sceneCreators_["EditScene"]         = [](ISceneArgs* args) { return std::make_unique<EditScene>(args); };

    loadableSceneCreators_["GameScene"] = [](ISceneArgs* args) { return std::make_unique<GameScene>(args); };
}

std::unique_ptr<SceneBase> SceneFactory::Create(const std::string& sceneName, ISceneArgs* pArgs)
{
    // シーン名に応じてシーンを生成
    auto it = sceneCreators_.find(sceneName);

    if (it != sceneCreators_.end())
    {
        return it->second(pArgs);
    }

    // シーン名が登録されていない場合
    this->OutputSceneMissingError(sceneName);
    return nullptr;
}

std::unique_ptr<ILoadableScene> SceneFactory::CreateLoadable(const std::string& sceneName, ISceneArgs* pArgs)
{
    // シーン名に応じてシーンを生成
    auto it = loadableSceneCreators_.find(sceneName);
    if (it != loadableSceneCreators_.end())
    {
        return it->second(pArgs);
    }

    // シーン名が登録されていない場合
    this->OutputSceneMissingError(sceneName);
    return nullptr;
}

void SceneFactory::OutputSceneMissingError(const std::string& sceneName)
{
    Logger::GetInstance()->LogError(
        __FILE__,
        __FUNCTION__,
        std::format("Scene Missing : {}", sceneName)
    );
    MessageBoxW(
        nullptr,
        std::format(L"シーン名 {} は登録されていません。", ConvertString(sceneName)).c_str(),
        L"Scene Creation Error",
        MB_OK | MB_ICONERROR
    );
}

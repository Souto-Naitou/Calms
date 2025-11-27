#include "SceneFactory.h"

#include <Scene/Title/TitleScene.h>
#include <Scene/Game/GameScene.h>
#include <Scene/Clear/ClearScene.h>
#include <Scene/GameOverTest/GameOverTestScene.h>
#include <scene/Edit/EditScene.h>
#include <Utility/ConvertString/ConvertString.h>

#include <cassert>
#include <format>
#include <scene/Load/LoadScene.h>
#include <DebugTools/Logger/Logger.h>
#include <cstdlib>

#define JUDGE_SCENE(class, arg) if (_sceneName == #class) { return std::make_unique<class>(arg); }

SceneFactory::SceneFactory()
{
    // シーンの登録
    sceneCreators_["TitleScene"] = [](ISceneArgs* args) { return std::make_unique<TitleScene>(args); };
    sceneCreators_["LoadScene"] = [](ISceneArgs* args) { return std::make_unique<LoadScene>(args); };
    sceneCreators_["GameScene"] = [](ISceneArgs* args) { return std::make_unique<GameScene>(args); };
    sceneCreators_["ClearScene"] = [](ISceneArgs* args) { return std::make_unique<ClearScene>(args); };
    sceneCreators_["GameOverTestScene"] = [](ISceneArgs* args) { return std::make_unique<GameOverTestScene>(args); };
    sceneCreators_["EditScene"] = [](ISceneArgs* args) { return std::make_unique<EditScene>(args); };
}

std::unique_ptr<SceneBase> SceneFactory::CreateScene(const std::string& sceneName, ISceneArgs* pArgs)
{
    // シーン名に応じてシーンを生成
    auto it = sceneCreators_.find(sceneName);

    if (it != sceneCreators_.end())
    {
        return it->second(pArgs);
    }

    // シーン名が登録されていない場合
    Logger::GetInstance()->LogError(
        __FILE__,
        __FUNCTION__,
        std::format("SceneMissing : {}", sceneName)
    );

    MessageBoxW(
        nullptr,
        std::format(L"シーン名 {} は登録されていません。", ConvertString(sceneName)).c_str(),
        L"Scene Creation Error",
        MB_OK | MB_ICONERROR
    );

    return nullptr;
}

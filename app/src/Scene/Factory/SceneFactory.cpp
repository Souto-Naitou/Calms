#include "SceneFactory.h"

#include <Scene/Title/TitleScene.h>
#include <Scene/Game/GameScene.h>
#include <Scene/Clear/ClearScene.h>
#include <Scene/GameOverTest/GameOverTestScene.h>
#include <Utility/ConvertString/ConvertString.h>

#include <cassert>
#include <format>
#include <scene/Load/LoadScene.h>

#define JUDGE_SCENE(class, arg) if (_sceneName == #class) { return std::make_unique<class>(arg); }

SceneFactory::SceneFactory()
{
    // シーンの登録
    sceneCreators_["TitleScene"] = [](ISceneArgs* args) { return std::make_unique<TitleScene>(args); };
    sceneCreators_["LoadScene"] = [](ISceneArgs* args) { return std::make_unique<LoadScene>(args); };
    sceneCreators_["GameScene"] = [](ISceneArgs* args) { return std::make_unique<GameScene>(args); };
    sceneCreators_["ClearScene"] = [](ISceneArgs* args) { return std::make_unique<ClearScene>(args); };
    sceneCreators_["GameOverTestScene"] = [](ISceneArgs* args) { return std::make_unique<GameOverTestScene>(args); };
}

std::unique_ptr<SceneBase> SceneFactory::CreateScene(const std::string& _sceneName, ISceneArgs* _pArgs)
{
    // シーン名に応じてシーンを生成
    try
    {
        return sceneCreators_.at(_sceneName)(_pArgs);
    }
    catch (const std::exception& e)
    {
        MessageBoxW(
            nullptr,
            std::format(L"シーン名 {} は登録されていません。", ConvertString(_sceneName)).c_str(),
            L"Scene Creation Error",
            MB_OK | MB_ICONERROR
        ); 
        assert(false && e.what());
        e;
    }

    return nullptr;
}

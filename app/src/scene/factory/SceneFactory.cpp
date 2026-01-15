#include "SceneFactory.h"

#include <Scene/Title/TitleScene.h>
#include <Scene/Game/GameScene.h>
#include <Scene/Clear/ClearScene.h>
#include <scene/Edit/EditScene.h>
#include <Utility/ConvertString/ConvertString.h>

#include <cassert>
#include <format>
#include <DebugTools/Logger/Logger.h>
#include <cstdlib>

#define JUDGE_SCENE(class, arg) if (_sceneName == #class) { return std::make_unique<class>(arg); }

SceneFactory::SceneFactory()
{
    // 繧ｷ繝ｼ繝ｳ縺ｮ逋ｻ骭ｲ
    sceneCreators_["TitleScene"] = [](ISceneArgs* args) { return std::make_unique<TitleScene>(args); };
    sceneCreators_["ClearScene"]        = [](ISceneArgs* args) { return std::make_unique<ClearScene>(args); };
    sceneCreators_["EditScene"]         = [](ISceneArgs* args) { return std::make_unique<EditScene>(args); };

    loadableSceneCreators_["GameScene"] = [](ISceneArgs* args) { return std::make_unique<GameScene>(args); };
}

std::unique_ptr<SceneBase> SceneFactory::Create(const std::string& sceneName, ISceneArgs* pArgs)
{
    // 繧ｷ繝ｼ繝ｳ蜷阪↓蠢懊§縺ｦ繧ｷ繝ｼ繝ｳ繧堤函謌・    auto it = sceneCreators_.find(sceneName);

    if (it != sceneCreators_.end())
    {
        return it->second(pArgs);
    }

    // 繧ｷ繝ｼ繝ｳ蜷阪′逋ｻ骭ｲ縺輔ｌ縺ｦ縺・↑縺・ｴ蜷・    this->OutputSceneMissingError(sceneName);
    return nullptr;
}

std::unique_ptr<ILoadableScene> SceneFactory::CreateLoadable(const std::string& sceneName, ISceneArgs* pArgs)
{
    // 繧ｷ繝ｼ繝ｳ蜷阪↓蠢懊§縺ｦ繧ｷ繝ｼ繝ｳ繧堤函謌・    auto it = loadableSceneCreators_.find(sceneName);
    if (it != loadableSceneCreators_.end())
    {
        return it->second(pArgs);
    }

    // 繧ｷ繝ｼ繝ｳ蜷阪′逋ｻ骭ｲ縺輔ｌ縺ｦ縺・↑縺・ｴ蜷・    this->OutputSceneMissingError(sceneName);
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
        std::format(L"繧ｷ繝ｼ繝ｳ蜷・{} 縺ｯ逋ｻ骭ｲ縺輔ｌ縺ｦ縺・∪縺帙ｓ縲・, ConvertString(sceneName)).c_str(),
        L"Scene Creation Error",
        MB_OK | MB_ICONERROR
    );
}

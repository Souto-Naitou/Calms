#include "SceneFactory.h"

#include <Scene/Title/TitleScene.h>
#include <Scene/Game/GameScene.h>
#include <Scene/Clear/ClearScene.h>

#include <cassert>

IScene* SceneFactory::CreateScene(const std::string& _sceneName)
{
    IScene* pNewScene = nullptr;

    if (_sceneName == "TitleScene")
    {
        pNewScene = new TitleScene();
    }
    else if (_sceneName == "GameScene")
    {
        pNewScene = new GameScene();
    }
    else if (_sceneName == "ClearScene")
    {
        pNewScene = new ClearScene();
    }

    assert(pNewScene);

    return pNewScene;
}

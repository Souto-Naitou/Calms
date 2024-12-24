#include "SceneFactory.h"

#include <Scene/Title/TitleScene.h>
#include <Scene/Game/GameScene.h>

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

    assert(pNewScene);

    return pNewScene;
}

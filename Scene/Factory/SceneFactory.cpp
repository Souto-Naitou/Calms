#include "SceneFactory.h"

#include <Scene/Title/TitleScene.h>


#include <cassert>

IScene* SceneFactory::CreateScene(const std::string& _sceneName)
{
    IScene* pNewScene = nullptr;

    if (_sceneName == "TitleScene")
    {
        pNewScene = new TitleScene();
    }

    assert(pNewScene);

    return pNewScene;
}

#include "SceneFactory.h"

#include <Scene/Title/TitleScene.h>
#include <Scene/Game/GameScene.h>
#include <Scene/Clear/ClearScene.h>
#include <Scene/Lobby/Lobby.h>
#include <Scene/MultiGame/MultiGame.h>

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
    else if (_sceneName == "LobbyScene")
    {
        pNewScene = new LobbyScene();
    }
    else if (_sceneName == "MultiGame")
    {
        pNewScene = new MultiGame();
    }

    assert(pNewScene);

    return pNewScene;
}

#include "SceneFactory.h"

#include <Scene/Title/TitleScene.h>
#include <Scene/Game/GameScene.h>
#include <Scene/Clear/ClearScene.h>
#include <Scene/Lobby/Lobby.h>
#include <Scene/MultiGame/MultiGame.h>

#include <cassert>

#define JUDGE_SCENE(class) if (_sceneName == #class) { return std::make_unique<class>(); }

std::unique_ptr<IScene> SceneFactory::CreateScene(const std::string& _sceneName)
{
    JUDGE_SCENE(TitleScene)
    else JUDGE_SCENE(GameScene)
    else JUDGE_SCENE(ClearScene)
    else JUDGE_SCENE(LobbyScene)
    else JUDGE_SCENE(MultiGame)

    assert(false && "シーンの生成に失敗しました");

    return nullptr;

}

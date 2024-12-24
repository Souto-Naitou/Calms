#pragma once

#include <Interfaces/IScene.h>
#include <Features/Object3d/Object3d.h>
#include <Features/GameEye/GameEye.h>
#include <GameObject/Player/Player.h>
#include <GameObject/PlayerBullet/PlayerBullet.h>
#include <list>
#include <memory>

class GameScene : public IScene
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 背景描画
    /// </summary>
    void Draw2dBackGround() override;

    /// <summary>
    /// 3D描画
    /// </summary>
    void Draw3d() override;

    /// <summary>
    /// 前景描画
    /// </summary>
    void Draw2dForeground() override;


private:
    std::unique_ptr<Object3d> grid_ = {};
    std::unique_ptr<GameEye> gameEye_ = {};
    std::unique_ptr<Player> player_ = {};
    std::list<std::unique_ptr<PlayerBullet>> playerBullets_ = {};


private:
    void CreatePlayerBullet();
    void RemovePlayerBullet();
};
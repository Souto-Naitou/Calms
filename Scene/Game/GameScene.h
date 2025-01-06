#pragma once

#include <Interfaces/IScene.h>
#include <DIContainer/DIContainer.h>
#include <Features/Object3d/Object3d.h>
#include <Features/GameEye/GameEye.h>
#include <GameObject/Player/Player.h>
#include <GameObject/PlayerBullet/PlayerBullet.h>
#include <GameObject/Enemy/Enemy.h>
#include <Common/structs.h>
#include <PopSystem/Enemy/EnemyPopSystem.h>
#include <DebugTools/DebugManager/DebugManager.h>
#include <UI/CountDown/CountDown.h>
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
    /// ライン描画
    /// </summary>
    void DrawLine() override;

    /// <summary>
    /// 前景描画
    /// </summary>
    void Draw2dForeground() override;


private:
    std::unique_ptr<Object3d>                   grid_               = {}; // !< グリッド
    std::unique_ptr<GameEye>                    gameEye_            = {}; // !< ゲームアイ
    std::unique_ptr<Player>                     player_             = {}; // !< プレイヤー
    std::list<std::unique_ptr<Enemy>>           enemy_              = {}; // !< 敵s
    std::list<std::unique_ptr<PlayerBullet>>    playerBullets_      = {}; // !< プレイヤー弾s

    EnemyPopSystem                              enemyPopSystem_     = {}; // !< 敵生成システム
    DIContainer                                 gObjDIContainer_    = {}; // !< ゲームオブジェクトのDIコンテナ
    DirectionalLight                            directionalLight_   = {}; // !< ディレクショナルライト
    PointLight                                  pointLight_         = {}; // !< ポイントライト
    std::unique_ptr<CountDown>                  countDown_          = {}; // !< カウントダウン


private: /// デバッグ用
    DebugManager* pDebugManager_ = nullptr;
    std::string name_ = "GameScene";

    /// コライダーの描画
    bool isDisplayColliderEnemy_ = false;
    bool isDisplayColliderPlayer_ = false;
    bool isDisplayColliderPlayerBullet_ = false;


private:
    void CreatePlayerBullet();
    void RemovePlayerBullet();

    void RemoveEnemy();
    void UpdateEnemyPopSystem();

    void DebugWindow();
};
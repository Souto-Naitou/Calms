#pragma once

#include <Scene/SceneBase.h>
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
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <GameObject/ScreenToWorld/ScreenToWorld.h>
#include <Features/RandomGenerator/RandomGenerator.h>
#include <GameTimer/GameTimer.h>
#include <InputGuide/InputGuide.h>
#include <Features/Line/Line.h>
#include <ScoreSystem/ScoreSystem.h>
#include <Features/Text/Text.h>
#include <list>
#include <memory>
#include <Features/Model/ModelManager.h>
#include <Core/DirectX12/TextureManager.h>

class GameScene : public SceneBase
{
public:
    GameScene(ISceneArgs* _args) : SceneBase(_args) {};

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
    /// 描画(テキスト描画を除く)
    /// </summary>
    void Draw() override;


    /// <summary>
    /// テキスト描画
    /// </summary>
    void DrawTexts() override;


private:
    

    std::unique_ptr<Object3d>                   grid_               = {};       // !< グリッド
    std::unique_ptr<GameEye>                    gameEye_            = {};       // !< ゲームアイ
    std::unique_ptr<Player>                     player_             = {};       // !< プレイヤー
    std::list<std::unique_ptr<Enemy>>           enemy_              = {};       // !< 敵s
    std::list<std::unique_ptr<PlayerBullet>>    playerBullets_      = {};       // !< プレイヤー弾s
    std::unique_ptr<ScreenToWorld>              screenToWorld_      = {};       // !< 座標変換
    std::unique_ptr<GameTimer>                  gameTimer_          = {};       // !< ゲームタイマー
    std::unique_ptr<InputGuide>                 inputGuide_         = {};       // !< 入力ガイド
    std::unique_ptr<ScoreSystem>                scoreSystem_        = {};       // !< スコアシステム
    std::unique_ptr<Text>                       fpsText_            = {};       // !< テキスト

    EnemyPopSystem                              enemyPopSystem_     = {};       // !< 敵生成システム
    DIContainer                                 gObjDIContainer_    = {};       // !< ゲームオブジェクトのDIコンテナ
    DirectionalLight                            directionalLight_   = {};       // !< ディレクショナルライト
    PointLight                                  pointLight_         = {};       // !< ポイントライト
    std::unique_ptr<CountDown>                  countDown_          = {};       // !< カウントダウン
    TimeMeasurer                                timer_              = {};       // !< タイマー
    double                                      countDownOffset_    = 2.0;      // !< カウントダウンのオフセット

    bool                                        isChangingScene_    = false;    // !< シーン遷移中かどうか
    std::unique_ptr<Line>                       lines_              = nullptr;  // !< エリア用ライン
    float                                       areaWidth_          = 25.0f;    // !< エリアの幅

    const uint32_t                              kMaxEnemyCount_     = 30;       // !< 最大敵数


private: /// デバッグ用
    DebugManager* pDebugManager_ = nullptr;
    std::string name_ = "GameScene";

    /// コライダーの描画
    bool isDisplayColliderEnemy_ = false;
    bool isDisplayColliderPlayer_ = false;
    bool isDisplayColliderPlayerBullet_ = false;

    float framerate_ = 60.0f;


private:
    void CreatePlayerBullet();
    void RemovePlayerBullet();

    void RemoveEnemy();
    void UpdateEnemyPopSystem();

    void PlayerSlowUpdate();

    void DebugWindow();


private:
    DeltaTimeManager* deltaTimeManager_ = nullptr;
    RandomGenerator* randomGenerator_ = nullptr;
    ModelManager* pModelManager_ = nullptr;
    LineSystem* pLineSystem_ = nullptr;
    TextureManager* pTextureManager_ = nullptr;
};
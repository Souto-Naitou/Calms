#pragma once

// engine
#include <Scene/SceneBase.h>
#include <Common/structs.h>
#include <Core/DirectX12/TextureManager.h>
#include <Features/GameEye/GameEye.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/RandomGenerator/RandomGenerator.h>
#include <Features/Line/Line.h>
#include <Features/Text/Text.h>
#include <Features/Model/ModelManager.h>
#include <drawable/object3d/Object3d.h>
#include <drawable/particle/Particle.h>
#include <DebugTools/DebugManager/DebugManager.h>
#include <Features/Bar2d/Bar2d.h>

// game
#include <entity/ScreenToWorld/ScreenToWorld.h>
#include <entity/player/Player.h>
#include <entity/playerbullet/PlayerBullet.h>
#include <entity/enemy/Enemy.h>
#include <ui/countdown/CountDown.h>
#include <ui/guide/InputGuide.h>
#include <logic/timer/InGameTimer.h>
#include <logic/spawner/EnemySpawner.h>

// stl
#include <vector>
#include <list>
#include <memory>
#include <array>
#include <entity/player/GameOverAnimation.h>

/// <summary>
/// ゲームシーン
/// </summary>
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

    /// <summary>
    /// シーン用のデバッグウィンドウを描画します。
    /// </summary>
    void ImGui();


private:
    enum class ParticleID
    {
        PlayerConstant,
        PlayerDeath,
        EnemyDeath,
        PlayerBullet,

        Size
    };

    static constexpr inline size_t kParticleIDMax = static_cast<size_t>(ParticleID::Size);

    std::unique_ptr<Canvas>                     canvasUI_           = {};       // !< UIキャンバス
    std::unique_ptr<Canvas>                     canvasGrid_         = {};       // !< Gridキャンバス
    std::unique_ptr<Canvas>                     canvas3dObject_     = {};       // !< MainCharactorキャンバス
    std::unique_ptr<Canvas>                     canvasParticle_     = {};       // !< パーティクルキャンバス

    std::unique_ptr<Object3d>                   grid_               = {};       // !< グリッド
    std::unique_ptr<GameEye>                    gameEye_            = {};       // !< ゲームアイ
    std::unique_ptr<Player>                     player_             = {};       // !< プレイヤー
    std::vector<std::unique_ptr<Enemy>>         enemies_            = {};       // !< 敵s
    std::list<std::unique_ptr<PlayerBullet>>    playerBullets_      = {};       // !< プレイヤー弾s
    std::unique_ptr<ScreenToWorld>              screenToWorld_      = {};       // !< 座標変換
    std::array<Particle*, kParticleIDMax>       particles_          = {};       // !< パーティクル
    /// UI
    std::unique_ptr<InGameTimer>                gameTimer_          = {};       // !< ゲームタイマー
    std::unique_ptr<InputGuide>                 inputGuide_         = {};       // !< 入力ガイド
    std::unique_ptr<Bar2d>                      healthBar_          = {};       // !< 体力バー

    std::unique_ptr<GameOverAnimation>          gameOverAnimation_  = {};       // !< ゲームオーバーアニメーション


    EntityCommonParams                          entityCommonParams_ = {};       // !< エンティティ共通パラメータ

    EnemySpawner                                enemyPopSystem_     = {};       // !< 敵生成システム
    DirectionalLight                            directionalLight_   = {};       // !< ディレクショナルライト
    PointLight                                  pointLight_         = {};       // !< ポイントライト
    std::unique_ptr<CountDown>                  countDown_          = {};       // !< カウントダウン
    TimeMeasurer                                timer_              = {};       // !< タイマー
    double                                      countDownOffset_    = 2.0;      // !< カウントダウンのオフセット

    bool                                        isChangingScene_    = false;    // !< シーン遷移中かどうか
    std::unique_ptr<Line>                       lines_              = nullptr;  // !< エリア用ライン
    float                                       areaWidth_          = 25.0f;    // !< エリアの幅

    const uint32_t                              kMaxEnemyCount_     = 120;       // !< 最大敵数

    TimeMeasurer     titleTimer_ = {}; // タイトル表示用タイマー


private: /// デバッグ用
    std::unique_ptr<DebugEntry<GameScene>> pDebugEntry_ = nullptr;

    /// コライダーの描画
    bool isDisplayColliderEnemy_ = false;
    bool isDisplayColliderPlayer_ = false;
    bool isDisplayColliderPlayerBullet_ = false;

    float framerate_ = 60.0f;


private:
    void CanvasInitialize();
    void ParticlesInitialize();
    void CreatePlayerBullet();
    void RemovePlayerBullet();
    void RemoveEnemy();
    void KillAllEnemies();

    /// <summary>
    /// </summary>
    void EnemyPopSystemUpdate();

    /// <summary>
    /// プレイヤーのスロー（低速移動）状態に関連する更新処理を行います。
    /// </summary>
    void PlayerSlowUpdate();


private:
    DeltaTimeManager* deltaTimeManager_ = nullptr;
    RandomGenerator* randomGenerator_ = nullptr;
    ModelManager* pModelManager_ = nullptr;
    LineSystem* pLineSystem_ = nullptr;
    TextureManager* pTextureManager_ = nullptr;
};
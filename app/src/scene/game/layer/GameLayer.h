#pragma once

#include <core/IGameLayer.h>

// engine
#include <Core/DirectX12/TextureManager.h>
#include <Common/structs.h>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <Features/Bar2d/Bar2d.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Layer/Canvas.h>
#include <Features/Lighting/PointLight/PointLight.h>
#include <Features/Line/Line.h>
#include <Features/Line/LineSystem.h>
#include <Features/Model/ModelManager.h>
#include <Features/RandomGenerator/RandomGenerator.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Layer/Layer.h>
#include <Interfaces/ISceneArgs.h>

// game
#include <drawable/object3d/Object3d.h>
#include <drawable/particle/Particle.h>
#include <entity/enemy/Enemy.h>
#include <entity/player/GameOverAnimation.h>
#include <entity/player/Player.h>
#include <entity/playerbullet/PlayerBullet.h>
#include <entity/screentoworld/ScreenToWorld.h>
#include <entity/explosion/PlayerExplosion.h>
#include <logic/spawner/EnemySpawner.h>
#include <logic/timer/InGameTimer.h>
#include <ui/countdown/CountDown.h>
#include <ui/guide/InputGuide.h>
#include <Features/event/EventSubscription.h>
#include <logic/event/PlayerExplosionEvent.h>

// stl
#include <cstdint>
#include <list>
#include <array>
#include <vector>
#include <memory>
#include <scene/game/animation/GameClearAnimation.h>
#include <drawable/sprite/Sprite.h>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <optional>

class GameLayer : public IGameLayer
{
public:
    void Initialize(ISceneArgs* pArgs, Layer* pLayer) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void ImGui();

private:
    void CanvasInitialize(ISceneArgs* pArgs);
    void LimitPlayerPosition();
    void ParticlesInitialize();
    void AddPlayerBullet();
    void RemovePlayerBullet();
    void RemoveDeadEnemy();
    void KillAllEnemies();
    void AddPlayerExplosion(const PlayerExplosionEvent&);
    void UpdatePlayerExplosion();

    /// <summary>
    /// </summary>
    void CreateEnemy();

    /// <summary>
    /// プレイヤーのスロー（低速移動）状態に関連する更新処理を行います。
    /// </summary>
    void PlayerSlowUpdate();

    enum class ParticleID
    {
        PlayerConstant,
        PlayerDeath,
        EnemyDeath,
        PlayerBullet,
        Background,

        Size
    };

    static constexpr inline size_t kParticleIDMax = static_cast<size_t>(ParticleID::Size);

#ifdef _DEBUG
    static constexpr inline uint32_t kGameLimitTime = 3600u;
#else
    static constexpr inline uint32_t kGameLimitTime = 60u;
#endif // _DEBUG

    std::unique_ptr<Canvas>                         canvasUI_               = {};       // !< UIキャンバス
    std::unique_ptr<Canvas>                         canvasGrid_             = {};       // !< Gridキャンバス
    std::unique_ptr<Canvas>                         canvas3dObject_         = {};       // !< MainCharactorキャンバス
    std::unique_ptr<Canvas>                         canvasParticle_         = {};       // !< パーティクルキャンバス
    std::unique_ptr<Canvas>                         canvasOverall_          = {};       // !< 全体キャンバス

    std::unique_ptr<Object3d>                       grid_                   = {};       // !< グリッド
    std::unique_ptr<GameEye>                        gameEye_                = {};       // !< ゲームアイ
    std::unique_ptr<Player>                         player_                 = {};       // !< プレイヤー
    std::vector<std::unique_ptr<Enemy>>             enemies_                = {};       // !< 敵s
    std::list<std::unique_ptr<PlayerBullet>>        playerBullets_          = {};       // !< プレイヤー弾s
    std::unique_ptr<ScreenToWorld>                  screenToWorld_          = {};       // !< 座標変換
    std::array<Particle*, kParticleIDMax>           particles_              = {};       // !< パーティクル
    std::vector<std::unique_ptr<PlayerExplosion>>   playerExplosions_       = {};       // !< プレイヤー爆発エフェクト
    /// UI
    std::unique_ptr<InGameTimer>                    gameTimer_              = {};       // !< ゲームタイマー
    std::unique_ptr<InputGuide>                     inputGuide_             = {};       // !< 入力ガイド
    std::unique_ptr<Bar2d>                          healthBar_              = {};       // !< 体力バー
    std::unique_ptr<Sprite>                         spriteClear_            = {};       // !< クリアスプライト
    std::unique_ptr<Sprite>                         spriteSpace_            = {};       // !< クリアスプライト

    std::unique_ptr<GameOverAnimation>              gameOverAnimation_      = {};       // !< ゲームオーバーアニメーション
    std::unique_ptr<GameClearAnimation>             gameClearAnimation_     = {};       // !< ゲームクリアアニメーション

    EntityCommonParams                              entityCommonParams_     = {};       // !< エンティティ共通パラメータ

    EnemySpawner                                    enemyPopSystem_         = {};       // !< 敵生成システム
    DirectionalLight                                directionalLight_       = {};       // !< ディレクショナルライト
    PointLight                                      pointLight_             = {};       // !< ポイントライト
    std::unique_ptr<CountDown>                      countDown_              = {};       // !< カウントダウン
    TimeMeasurer                                    timer_                  = {};       // !< タイマー
    double                                          countDownOffset_        = 2.0;      // !< カウントダウンのオフセット

    bool                                            isEnding_               = false;    // !< ゲーム終了フラグ
    bool                                            isChangingScene_        = false;    // !< シーン遷移中かどうか
    std::unique_ptr<Line>                           lines_                  = nullptr;  // !< エリア用ライン
    float                                           areaWidth_              = 25.0f;    // !< エリアの幅
    const uint32_t                                  kMaxEnemyCount_         = 120;      // !< 最大敵数
    TimeMeasurer                                    titleTimer_             = {};       // !< タイトル表示用タイマー

    std::optional<EventSubscription>                playerExplosionSub_     = std::nullopt;

    // Pointers
    DirectX12*          pDx12_              = nullptr;
    DeltaTimeManager*   deltaTimeManager_   = nullptr;
    RandomGenerator*    randomGenerator_    = nullptr;
    ModelManager*       pModelManager_      = nullptr;
    LineSystem*         pLineSystem_        = nullptr;
    TextureManager*     pTextureManager_    = nullptr;
    Layer*              pLayer_             = nullptr;

    /// [ デバッグ ]
    std::unique_ptr<DebugEntry<GameLayer>> pDebugEntry_ = nullptr;
    bool isDisplayColliderEnemy_        = false;
    bool isDisplayColliderPlayer_       = false;
    bool isDisplayColliderPlayerBullet_ = false;
    float framerate_                    = 60.0f;
};
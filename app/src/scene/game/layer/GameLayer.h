#pragma once

#include <core/ISceneLayer.h>

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
#include <Features/Layer/OrderedCanvasLayer.h>
#include <Features/event/EventSubscription.h>
#include <Interfaces/ISceneArgs.h>

// post effects
#include <Effects/PostEffects/Grayscale/Grayscale.h>

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
#include <logic/event/PlayerExplosionEvent.h>
#include <logic/score/ScoreCalculator.h>
#include <ui/countdown/CountDown.h>
#include <ui/guide/InputGuide.h>
#include <ui/PlayerUI3d.h>

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
#include <Effects/PostEffects/Grayscale/Grayscale.h>

/// <summary>
/// ゲーム層 (他にポーズメニュー層やリザルト層などを実装予定)
/// </summary>
class GameLayer : public ILoadableGameLayer
{
public:
    void Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer) override;
    void Finalize() override;
    void Update() override;
    void Draw() override;
    void Preload(const PreloadContext& ctx, TaskExecutor& executor) override;

private:
    void CanvasInitialize(TaskExecutor& executor, ISceneArgs* pArgs);
    void LimitPlayerPosition();
    void ParticlesInitialize();
    void SpritesInitialize();
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

    static constexpr inline size_t kMaxParticleIDs_ = static_cast<size_t>(ParticleID::Size);
    static constexpr inline float  kGameEyeHeightDefault_ = 65.0f;
    static constexpr inline float  kGameEyeHeightDuringSlow_ = 30.0f;
    static constexpr inline float  kTargetDirectionalLightFlashIntensity_ = 12.0f;

#ifdef _DEBUG
    static constexpr inline uint32_t kGameLimitTime = 3200u;
#else
    static constexpr inline uint32_t kGameLimitTime = 60u;
#endif // _DEBUG

    std::unique_ptr<Canvas>                         canvasBackground_       = {};       // !< 背景キャンバス
    std::unique_ptr<Canvas>                         canvasUI_               = {};       // !< UIキャンバス
    std::unique_ptr<Canvas>                         canvasGrid_             = {};       // !< Gridキャンバス
    std::unique_ptr<Canvas>                         canvas3dObject_         = {};       // !< MainCharactorキャンバス
    std::unique_ptr<Canvas>                         canvasParticle_         = {};       // !< パーティクルキャンバス
    std::unique_ptr<Canvas>                         canvasOverall_          = {};       // !< 全体キャンバス
    std::unique_ptr<Canvas>                         canvasUIEffected_       = {};       // !< ラインキャンバス

    std::unique_ptr<Object3d>                       grid_                   = {};       // !< グリッド
    std::unique_ptr<GameEye>                        pGameEye_               = {};       // !< ゲームアイ
    std::unique_ptr<Player>                         pPlayer_                = {};       // !< プレイヤー
    std::vector<std::unique_ptr<Enemy>>             enemies_                = {};       // !< 敵s
    std::list<std::unique_ptr<PlayerBullet>>        playerBullets_          = {};       // !< プレイヤー弾s
    std::unique_ptr<ScreenToWorld>                  screenToWorld_          = {};       // !< 座標変換
    std::array<Particle*, kMaxParticleIDs_>         particles_              = {};       // !< パーティクル
    std::vector<std::unique_ptr<PlayerExplosion>>   playerExplosions_       = {};       // !< プレイヤー爆発エフェクト
    std::unique_ptr<ScoreCalculator>                scoreCalculator_        = {};       // !< スコア計算機
    /// UI
    std::unique_ptr<InGameTimer>                    ingameTimer_            = {};       // !< ゲームタイマー
    std::unique_ptr<InputGuide>                     inputGuide_             = {};       // !< 入力ガイド
    std::unique_ptr<Sprite>                         spriteClear_            = {};       // !< クリアスプライト
    std::unique_ptr<Sprite>                         spriteSpace_            = {};       // !< クリアスプライト
    std::unique_ptr<PlayerUI3d>                     pPlayerUI3d_            = {};       // !< プレイヤー3DUI

    std::unique_ptr<GameOverAnimation>              gameOverAnimation_      = {};       // !< ゲームオーバーアニメーション
    std::unique_ptr<GameClearAnimation>             pGameClearAnimation_    = {};       // !< ゲームクリアアニメーション

    EntityCommonParams                              entityCommonParams_     = {};       // !< エンティティ共通パラメータ

    EnemySpawner                                    enemyPopSystem_         = {};       // !< 敵生成システム
    DirectionalLight                                directionalLight_       = {};       // !< ディレクショナルライト
    PointLight                                      pointLight_             = {};       // !< ポイントライト
    std::unique_ptr<CountDown>                      pStartCountDown_        = {};       // !< カウントダウン
    TimeMeasurer                                    timer_                  = {};       // !< タイマー
    double                                          countDownOffset_        = 2.0;      // !< カウントダウンのオフセット

    bool                                            isGameStartFlashed_     = false;    // !< ゲーム開始フラッシュ完了フラグ
    bool                                            isEnding_               = false;    // !< ゲーム終了フラグ
    bool                                            isChangingScene_        = false;    // !< シーン遷移中かどうか
    std::unique_ptr<Line>                           lines_                  = nullptr;  // !< エリア用ライン
    float                                           areaWidth_              = 25.0f;    // !< エリアの幅
    const uint32_t                                  kMaxEnemyCount_         = 120;      // !< 最大敵数
    Audio*                                          pBGM_                   = nullptr;  // !< BGMポインタ

    std::optional<EventSubscription>                playerExplosionSub_     = std::nullopt;

    // Pointers
    DirectX12*          pDx12_              = nullptr;
    DeltaTimeManager*   pDeltaTimeManager_   = nullptr;
    RandomGenerator*    randomGenerator_    = nullptr;
    ModelManager*       pModelManager_      = nullptr;
    LineSystem*         pLineSystem_        = nullptr;
    TextureManager*     pTextureManager_    = nullptr;
    OrderedCanvasLayer* pLayer_             = nullptr;
    GrayscaleOption*    pOptionGrayscale_   = nullptr;

    /// [ デバッグ ]
    bool isDisplayColliderEnemy_        = false;
    bool isDisplayColliderPlayer_       = false;
    bool isDisplayColliderPlayerBullet_ = false;
    float framerate_                    = 60.0f;
};
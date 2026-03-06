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
#include <drawable/line/Line.h>
#include <drawable/line/LineSystem.h>
#include <drawable/object3d/Object3d.h>
#include <drawable/particle/Particle.h>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <drawable/particle/emitter/ParticleEmitterGroup.h>
#include <drawable/sprite/Sprite.h>
#include <Features/Model/ModelManager.h>
#include <Features/RandomGenerator/RandomGenerator.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Layer/OrderedCanvasLayer.h>
#include <Features/event/EventSubscription.h>
#include <Interfaces/ISceneArgs.h>

// PostEffects
#include <Effects/PostEffects/Grayscale/Grayscale.h>

// Game
#include <entity/player/GameOverAnimation.h>
#include <entity/player/Player.h>
#include <entity/generator/PlayerBulletGenerator.h>
#include <entity/playerbullet/PlayerBullet.h>
#include <entity/screentoworld/ScreenToWorld.h>
#include <entity/explosion/PlayerExplosion.h>
#include <entity/enemy/EnemyFactory.h>
#include <entity/enemy/EnemyRepository.h>
#include <logic/spawner/EnemySpawner.h>
#include <logic/timer/InGameCountDown.h>
#include <logic/event/PlayerExplosionEvent.h>
#include <logic/score/ScoreCalculator.h>
#include <logic/slomo/SlomoLogic.h>
#include <ui/countdown/CountDown.h>
#include <ui/guide/InputGuide.h>
#include <ui/PlayerUI3d.h>
#include <scene/game/animation/GameClearAnimation.h>
#include <presentation/slomo/SlomoEffectController.h>

// STL
#include <cstdint>
#include <list>
#include <array>
#include <vector>
#include <memory>
#include <optional>
#include <presentation/animation/RadialBeat.h>

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
    void ImGui();
    Canvas* GetOverallCanvas() const { return canvasOverall_.get(); }
    void OnSceneChangeReserved();

private:
    void CanvasInitialize(TaskExecutor& executor, ISceneArgs* pArgs);
    void ParticlesInitialize();
    void SpritesInitialize();
    void AddPlayerBullet();
    void RemovePlayerBullet();
    void AddPlayerExplosion(const PlayerExplosionEvent&);
    void UpdatePlayerExplosion();
    void RegisterParticleEmitters();

    /// <summary>
    /// </summary>
    void CreateEnemy();

    /// <summary>
    /// プレイヤーのスロー（低速移動）状態に関連する更新処理を行います。
    /// </summary>
    void UpdateSlomo();

    /// <summary>
    /// パーティクルの種類を識別するための列挙型
    /// </summary>
    enum class ParticleID
    {
        PlayerConstant,
        PlayerDeath,
        Triangle,
        Spark,
        Background,

        Size
    };

    static constexpr inline size_t kMaxParticleIDs_ = static_cast<size_t>(ParticleID::Size);
    static constexpr inline size_t kMaxPlayerBullets = 128u;
    static constexpr inline float  kGameEyeHeightDefault_ = 65.0f;
    static constexpr inline float  kGameEyeHeightDuringSlow_ = 30.0f;
    static constexpr inline float  kTargetDirectionalLightFlashIntensity_ = 12.0f;

#ifdef _DEBUG
    static constexpr inline uint32_t kGameLimitTime = 15u;
#else
    static constexpr inline uint32_t kGameLimitTime = 60u;
#endif // _DEBUG

    std::unique_ptr<DebugEntry<GameLayer>>          pDebugEntry_            = nullptr;  // !< デバッグエントリ

    /// Canvases
    std::unique_ptr<Canvas>                         canvasBackground_       = {};       // !< 背景キャンバス
    std::unique_ptr<Canvas>                         canvasUI_               = {};       // !< UIキャンバス
    std::unique_ptr<Canvas>                         canvasGrid_             = {};       // !< Gridキャンバス
    std::unique_ptr<Canvas>                         canvas3dObject_         = {};       // !< MainCharactorキャンバス
    std::unique_ptr<Canvas>                         canvasParticle_         = {};       // !< パーティクルキャンバス
    std::unique_ptr<Canvas>                         canvasOverall_          = {};       // !< 全体キャンバス
    std::unique_ptr<Canvas>                         canvasUIEffected_       = {};       // !< ラインキャンバス
    
    /// Enemy
    std::unique_ptr<EnemyRepository>                pEnemyRepository_       = {};       // !< 敵リポジトリ
    std::unique_ptr<EnemySpawner>                   pEnemyPopSystem_        = {};       // !< 敵生成システム
    std::unique_ptr<EnemyFactory>                   pEnemyFactory_          = {};       // !< 敵生成ファクトリ

    std::unique_ptr<Object3d>                       pGrid_                  = {};       // !< グリッド
    std::unique_ptr<GameEye>                        pGameEye_               = {};       // !< ゲームアイ
    std::unique_ptr<Player>                         pPlayer_                = {};       // !< プレイヤー
    std::vector<std::unique_ptr<PlayerBullet>>      playerBullets_          = {};       // !< プレイヤー弾s
    std::unique_ptr<ScreenToWorld>                  screenToWorld_          = {};       // !< 座標変換
    std::vector<std::unique_ptr<PlayerExplosion>>   playerExplosions_       = {};       // !< プレイヤー爆発エフェクト
    std::unique_ptr<ScoreCalculator>                scoreCalculator_        = {};       // !< スコア計算機

    /// UI
    std::unique_ptr<InGameCountDown>                    ingameTimer_            = {};       // !< ゲームタイマー
    std::unique_ptr<InputGuide>                     inputGuide_             = {};       // !< 入力ガイド
    std::unique_ptr<Sprite>                         spriteClear_            = {};       // !< クリアスプライト
    std::unique_ptr<Sprite>                         spriteSpace_            = {};       // !< クリアスプライト
    std::unique_ptr<PlayerUI3d>                     pPlayerUI3d_            = {};       // !< プレイヤー3DUI

    std::unique_ptr<GameOverAnimation>              gameOverAnimation_      = {};       // !< ゲームオーバーアニメーション
    std::unique_ptr<GameClearAnimation>             pGameClearAnimation_    = {};       // !< ゲームクリアアニメーション
    std::unique_ptr<SlomoLogic>                     pSlomoLogic_            = {};       // !< スロー移動ロジック
    std::unique_ptr<SlomoEffectController>          pSlomoEffect_           = {};       // !< スロー移動ロジック
    std::unique_ptr<RadialBeat>                     pRadialBeat_            = nullptr;  // !< 放射状ブラービート

    PlayerBulletGenerator                           playerBulletGenerator_  = {};       // !< プレイヤー弾生成システム
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

    AABB                                            playableArea_           = {};       // !< プレイヤーの移動可能範囲

    std::optional<EventSubscription>                playerExplosionSub_     = std::nullopt;
    std::optional<EventSubscription>                particleEmitSub_        = std::nullopt;
    std::array<Particle*, kMaxParticleIDs_>         particles_              = {};       // !< パーティクル
    std::unique_ptr<ParticleEmitterGroup>           pEmitterGroup_          = nullptr;  // !< エミッターグループ

    // Pointers
    DirectX12*          pDx12_              = nullptr;
    DeltaTimeManager*   pDeltaTimeManager_  = nullptr;
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
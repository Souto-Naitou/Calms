#pragma once

#include <Entity/EntityBase.h>
#include <drawable/object3d/Object3d.h>
#include <memory>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <Features/Audio/AudioManager.h>
#include <Features/Audio/Audio.h>
#include <Features/Model/ModelManager.h>
#include <Features/Model/IModel.h>
#include "PlayerInput.h"
#include "PlayerMovement.h"
#include "PlayerExplosionTrigger.h"


/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public EntityBase
{
public:
    struct Params
    {
        Particle* particle = nullptr;
        ModelManager* pModelManager = nullptr;
    };

    enum class Flags : uint32_t
    {
        None            = 0x00000000,
        DisableMovement = 0x00000001,
        DisableInput    = 0x00000002,
    };

    Player(const Params& params);

    /// <summary>
    /// プレイヤーの初期化を行います。
    /// 必要なモデルやコライダー、入出力の初期設定を行います。
    /// </summary>
    /// <param name="enableDebugWindow">デバッグウィンドウの有効/無効。</param>
    void Initialize(const EntityCommonParams& params, bool enableDebugWindow = true) override;

    /// <summary>
    /// プレイヤーの終了処理を行います。
    /// リソースの解放などを想定しています。
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// プレイヤーの状態更新を行います。
    /// 入力処理、移動、当たり判定などの更新を行います。
    /// </summary>
    void Update() override;

    /// <summary>
    /// プレイヤーの描画処理を行います。
    /// </summary>
    void Draw1F() override;

    /// <summary>
    /// プレイヤーのライン等の補助描画を行います。
    /// </summary>
    void DrawLine() override;

    /// <summary>
    /// ImGui を用いたデバッグUIの描画を行います。
    /// </summary>
    void ImGui() override;


public: /// Getter
    bool IsShot() const { return isShot_; }
    bool IsSlow() const { return pInput_->GetData().isSlowPressed; }
    Object3d* GetObject3d() const { return object_.get(); }

public: /// Setter
    void DisableMovement();
    void DisableInput();


private:
    /// <summary>
    /// トリガー型の衝突イベント時に呼ばれます。
    /// </summary>
    /// <param name="other">衝突相手のコライダー。</param>
    void OnCollisionTrigger(const Collider* other);

    /// <summary>
    /// 物理衝突時に呼ばれます。
    /// </summary>
    /// <param name="other">衝突相手のコライダー。</param>
    void OnCollision(const Collider* other);

    /// <summary>
    /// モデルや描画オブジェクトなどプレイヤー関連オブジェクトを初期化します。
    /// </summary>
    void ObjectsInitialize();

    /// <summary>
    /// コライダーの生成と設定を行います。
    /// </summary>
    void ColliderInitialize();

    /// <summary>
    /// パーティクルエミッターの初期化を行います。
    /// </summary>
    void ParticleEmittersInitialize();

    /// <summary>
    /// 入力状態から移動や射撃などのコマンドを更新します。
    /// </summary>
    void UpdateInputCommands();

    /// <summary>
    /// オーディオハンドルの初期化を行います。
    /// </summary>
    void AudioHandleInitialize();

    void ComponentInitialize();

    // 初期化パラメータ
    Params                          params_;
    uint32_t                        flags_      = static_cast<uint32_t>(Flags::None);

    std::unique_ptr<PlayerInput>            pInput_             = nullptr;
    std::unique_ptr<PlayerMovement>         pMovement_          = nullptr;
    std::unique_ptr<PlayerExplosionTrigger> pExplosionTrigger_  = nullptr;

    std::unique_ptr<TimeMeasurer>           timerShot_          = {};
    std::unique_ptr<IModel>                 pModelSelfBody_     = nullptr;
    std::unique_ptr<Object3d>               object_             = {};

    float movePower_ = 0.0f;
    float shotInterval_ = 0.05f;
    bool isShot_ = false;

    /// コライダー
    std::unique_ptr<Collider> collider_ = nullptr;
    OBB obb_ = {};

    /// 反発用
    Vector3 accelerationRefl_ = {};
    float reflectionPower_ = 70.0f;

    /// パーティクルエミッター
    std::unique_ptr<ParticleEmitter> emitterConstant_ = nullptr;    // 常時発生エミッター

    /// オーディオ
    Audio* audioShot_ = nullptr;
    Audio* audioDeath_ = nullptr;
    Audio* audioSlowOn_ = nullptr;
    Audio* audioSlowOff_ = nullptr;


private: /// 他クラスの所有物
    CollisionManager* collisionManager_ = nullptr;
    DeltaTimeManager* deltaTimeManager_ = nullptr;
    AudioManager* audioManager_ = nullptr;
    ModelManager* pModelManager_ = nullptr;
};
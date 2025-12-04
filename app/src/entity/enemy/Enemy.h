#pragma once

#include <Entity/EntityBase.h>
#include <drawable/object3d/Object3d.h>
#include <Vector3.h>
#include <memory>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Primitive/OBB.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <Features/Audio/AudioManager.h>
#include <drawable/particle/Particle.h>

/// <summary>
/// 敵クラス
/// </summary>
class Enemy : public EntityBase
{
public:
    struct Params
    {
        Particle*   pParticleTriangle   = nullptr;      // デスパーティクル
        Particle*   pParticleCircle     = nullptr;      // デスパーティクル
        IModel*     pModelSelfBody      = nullptr;      // 本体モデル
    };

public:
    Enemy(const Params& _desc);

    /// <summary>
    /// 敵の初期化を行います。
    /// モデル・コライダー・各種システムとの接続を準備します。
    /// </summary>
    /// <param name="_enableDebugWindow">デバッグウィンドウの有効/無効。</param>
    void Initialize(const EntityCommonParams& params, bool enableDebugWindow = true) override;

    /// <summary>
    /// 終了処理を行います。
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 敵の状態更新を行います。
    /// 移動、当たり判定、寿命管理などを更新します。
    /// </summary>
    void Update() override;

    /// <summary>
    /// 敵の描画処理を行います。
    /// </summary>
    void Draw1F() override;

    /// <summary>
    /// 敵のライン等の補助描画を行います。
    /// </summary>
    void DrawLine();

    /// <summary>
    /// ImGuiを用いたデバッグUIの描画を行います。
    /// </summary>
    void ImGui() override;


public: /// Setter
    void SetLocationProvider(EntityBase* _gameObject) { locationProvider_ = _gameObject; }
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }


private:
    /// <summary>
    /// モデルや表示オブジェクトを初期化します。
    /// </summary>
    void InitializeObjects();

    /// <summary>
    /// コライダーを初期化します。
    /// </summary>
    void InitializeCollider();

    /// <summary>
    /// パーティクルエミッターを初期化します。
    /// </summary>
    void InitializeParticleEmitters();

    /// <summary>
    /// 変換（位置・回転・スケール）を更新します。
    /// </summary>
    void UpdateTransform();

    /// <summary>
    /// 当たり判定の更新を行います。
    /// </summary>
    void UpdateCollider();

    /// <summary>
    /// 表示オブジェクトの更新を行います。
    /// </summary>
    void UpdateObjects();

    /// <summary>
    /// 物理衝突時に呼ばれます。
    /// </summary>
    /// <param name="_other">衝突相手のコライダー。</param>
    void OnCollision(const Collider* _other);

    /// <summary>
    /// トリガー衝突時に呼ばれます。
    /// </summary>
    /// <param name="_other">衝突相手のコライダー。</param>
    void OnCollisionTrigger(const Collider* _other);

    Params          params_                     = {};

    std::unique_ptr<Object3d>           objectSelfBody_     = {};
    std::unique_ptr<TimeMeasurer>       timeMeasurer_       = {};

    float           lifeTimeLimit_              = 3.0f;
    Vector3         accelerationRefl_           = {};
    Vector3         moveVelocity_               = {};
    Vector2         distanceToTarget            = {};
    EntityBase*     locationProvider_           = nullptr;
    Vector2         positionTarget_             = {};
    Vector2         velocity_move               = {};
    float           moveSpeed_                  = 0.0f;
    float           reflectionPower_            = 15.0f;
    float           bulletReflectionPower_      = 40.0f;

    // Collision
    std::unique_ptr<Collider>   collider_            = nullptr;
    OBB                         obb_                 = {};
    bool                        isDrawCollisionArea_ = false;

    Audio* audioDeath_  = nullptr;

    /// パーティクル
    std::unique_ptr<ParticleEmitter>    pParticleDeathShort_             = nullptr;
    std::unique_ptr<ParticleEmitter>    pParticleDeathSplatter_     = nullptr;
    std::unique_ptr<IModel>             pModelSelfBody_             = nullptr;


private: /// 他クラスの所有物
    CollisionManager*   collisionManager_   = nullptr;
    DeltaTimeManager*   deltaTimeManager_   = nullptr;
    AudioManager*       audioManager_       = nullptr;
};
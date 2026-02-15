#pragma once

#include <Entity/EntityBase.h>
#include <drawable/object3d/Object3d.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <Features/Audio/AudioManager.h>
#include <drawable/particle/Particle.h>
#include <Features/Primitive/SphereLine.h>
#include <Features/Primitive/Sphere.h>
#include <Vector3.h>
#include "./EnemyTypes.h"
#include <component/FollowMovement.h>
#include <memory>
#include <component/FocusOrientation.h>
#include <common/structs.h>
#include <Features/Lighting/PointLight/PointLight.h>
#include <entity/status/EntityStats.h>

/// <summary>
/// 敵クラス(ノーマル)
/// </summary>
class Enemy : public EntityBase
{
public:
    struct Params
    {
        DirectionalLight*   pDirLight           = nullptr;
        PointLight*         pPointLight         = nullptr;
        Particle*           pParticleTriangle   = nullptr;      // デスパーティクル
        Particle*           pParticleCircle     = nullptr;      // デスパーティクル
        IModel*             pModelSelfBody      = nullptr;      // 本体モデル
        Vector3             initPosition        = {};           // 初期位置
        const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
    };

public:
    Enemy(const Params& _desc);

    /// <summary>
    /// 敵の初期化を行います。
    /// モデル・コライダー・各種システムとの接続を準備します。
    /// </summary>
    /// <param name="_enableDebugWindow">デバッグウィンドウの有効/無効。</param>
    void Initialize(bool enableDebugWindow = true) override;

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
    /// ImGuiを用いたデバッグUIの描画を行います。
    /// </summary>
    void ImGui() override;

    /// Setter
    void SetIsDrawCollisionArea(bool isDraw) { isDrawCollisionArea_ = isDraw; }

private:
    /// <summary>
    /// コンポーネントを初期化します。
    /// </summary>
    void InitializeComponents();

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
    void OnCollision(const Collider* other);

    /// <summary>
    /// トリガー衝突時に呼ばれます。
    /// </summary>
    /// <param name="_other">衝突相手のコライダー。</param>
    void OnCollisionTrigger(const Collider* other);

private:
    static constexpr EnemyTypes kEnemyType_         = EnemyTypes::Normal;
    static constexpr float      kFollowSpeed_       = 10.0f;
    static constexpr float      kFriction_          = 0.95f;
    static constexpr float      kReflectionPower_   = 15.0f;
    static constexpr float      kCameraShakePower_  = 0.1f;

    /// [ 初期化パラメータ ]
    Params  params_ = {};

    /// [ コンポーネント ]
    EulerTransform                          transform_          = {};
    std::unique_ptr<Object3d>               pObjectSelfBody_    = {};
    std::unique_ptr<IModel>                 pModelSelfBody_     = nullptr;
    std::unique_ptr<TimeMeasurer>           pTimeMeasurer_      = {};
    std::unique_ptr<EntityStats>            pStats_             = nullptr;
    std::unique_ptr<FollowMovement>         pMovement_          = nullptr;
    std::unique_ptr<FocusOrientation>       pFocusOrientation_  = nullptr;
    std::unique_ptr<Collider>               pCollider_          = nullptr;

    /// [ コライダー付随データ ]
    Sphere      sphere_                 = {};
    SphereLine  sphereLine_             = {};
    bool        isDrawCollisionArea_    = false;

    /// [ SE ]
    Audio*  audioDeath_  = nullptr;

    /// [ パーティクルエミッタ ]
    std::unique_ptr<ParticleEmitter>    pParticleDeathShort_        = nullptr;
    std::unique_ptr<ParticleEmitter>    pParticleDeathSplatter_     = nullptr;

    /// [ 他クラスの所有物 ]
    CollisionManager*   pCollisionManager_  = nullptr;
    DeltaTimeManager*   pDeltaTimeManager_  = nullptr;
    AudioManager*       pAudioManager_      = nullptr;
};
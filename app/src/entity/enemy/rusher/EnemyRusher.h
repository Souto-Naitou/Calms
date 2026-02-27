#pragma once
#include <entity/EntityBase.h>
#include <Features/Collision/Collider/Collider.h>
#include <drawable/object3d/Object3d.h>
#include <memory>
#include <component/IMovement.h>
#include <component/FollowMovement.h>
#include <component/DashMovementLinear.h>
#include <Math/Transform.h>
#include <component/FocusOrientation.h>
#include <entity/enemy/Rusher/EnemyRusherState.h>
#include <entity/enemy/EnemyInitParams.h>
#include "nima_engine/src/Features/Primitive/Sphere.h"
#include "../../Status/EntityStats.h"
#include <nima_engine/modules/vectormatrix/math/Color.h>

class EnemyRusher : public EntityBase
{
public:
    static constexpr RGBA   kColorDefault_      = RGBA(0x17485cff);
    static constexpr RGBA   kColorDashing_      = RGBA(0x40c4faff);
    static constexpr float  kDashDistance_      = 15.0f;                // ダッシュ距離
    static constexpr float  kDashDurationSec_   = 0.8f;                 // ダッシュ時間(秒)

    EnemyRusher(const EnemyRusherInitParams& params) : params_(params) {}

    void Initialize(bool enableDebugWindow = true) override;
    void Finalize() override;
    void Update() override;
    void Draw1F() override;
    void ImGui() override;

    void OnCollisionTrigger(const Collider* pOther);
    void OnCollision(const Collider* pOther);

    /// <summary>
    /// 状態を変更します
    /// </summary>
    void ChangeState(std::unique_ptr<EnemyRusherState> newState);

    /// [ 行動インターフェース ]

    /// <summary>
    /// ターゲットに向かって注視します。
    /// </summary>
    /// <param name="deltaTime"></param>
    void FocusOnTarget(float deltaTime);

    /// <summary>
    /// ターゲットに向かってダッシュ移動を開始します。
    /// </summary>
    void DashToTarget();

    /// <summary>
    /// 追尾移動に切り替えます。
    /// </summary>
    void ToFollowMovement();

    /// <summary>
    /// ダッシュ移動に切り替えます。
    /// </summary>
    void ToDashMovement();

    /// <summary>
    /// 物理移動に切り替えます。
    /// </summary>
    void ToPhysicsMovement();

    /// <summary>
    /// 移動を無効化します。
    /// </summary>
    void DisableMovement();

    /// <summary>
    /// 色を変更します。
    /// </summary>
    /// <param name="color"></param>
    void ChangeColor(const Vector4& color);
    
    /// [ 判定インターフェース ]

    /// <summary>
    /// ターゲットに近づいているか判定します。
    /// </summary>
    /// <param name="thresholdDistance">近いと判定する距離の閾値。</param>
    bool IsCloseToTarget(float thresholdDistance) const;

    /// <summary>
    /// ダッシュ中か判定します。
    /// </summary>
    bool IsDashing() const;

    bool IsStopped() const;

    /// <summary>
    /// ダッシュ中の経過時間を取得します。
    /// </summary>
    float GetDashElapsedTime() const;

private:
    void InitializeState();
    void InitializeComponents();
    // コンポーネントの初期化
    void InitializeTransform();
    void InitializeBody();
    void InitializeMovement();
    void InitializeFocusOrientation();
    void InitializeStats();
    void InitializeCollider(EntityStats* pStats);

    static constexpr float  kFollowSpeed_                       = 5.0f;     // 追尾速度(スカラー)
    static constexpr float  kReflectionPower_                   = 15.0f;
    static constexpr float  kReflectionPowerPlayerExplosion_    = 10.0f;
    static constexpr float  kCameraShakePower_                  = 0.1f;

    std::unique_ptr<EnemyRusherState> pCurrentState_ = nullptr;

    EnemyRusherInitParams           params_;
    EulerTransform                  transform_          = {};
    std::unique_ptr<Collider>       pCollider_          = nullptr;
    std::unique_ptr<Object3d>       pObjectSelfBody_    = {};
    std::unique_ptr<Sphere>         pSphere_            = {};
    std::unique_ptr<EntityStats>    pStats_             = nullptr;

    // 移動コンポーネントを差し替えて使うためのポインタ
    IMovement* pCurrentMovement_ = nullptr;
    
    std::unique_ptr<PhysicsMovement>    pPhysicsMovement_   = nullptr;
    std::unique_ptr<FollowMovement>     pFollowMovement_    = nullptr;
    std::unique_ptr<DashMovementLinear> pDashMovement_      = nullptr;
    std::unique_ptr<FocusOrientation>   pFocusOrientation_  = nullptr;
};
#include "EnemyRusher.h"
#include "nima_engine/src/Features/DeltaTimeManager/DeltaTimeManager.h"
#include "EnemyRusherStateFollow.h"
#include <Color.h>
#include "nima_engine/src/Features/Collision/Manager/CollisionManager.h"
#include <functional>
#include <logic/event/KillEnemyEvent.h>
#include "nima_engine/src/Features/event/EventListener.h"



void EnemyRusher::Initialize(bool enableDebugWindow /*= true*/)
{
    EntityBase::Initialize(enableDebugWindow);
    EntityBase::SetName(utl::debug::generate_name("EnemyRusher", this));

    this->InitializeState();
    this->InitializeComponents();
}

void EnemyRusher::Finalize()
{
    CollisionManager::GetInstance()->UnregisterCollider(pCollider_.get());
}

void EnemyRusher::Update()
{
    /// デルタタイム取得
    constexpr auto channel = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    float deltaTime = DeltaTimeManager::GetInstance()->GetDeltaTime(channel);

    /// 状態更新 (Follow → Aim → Dash → Follow → ...)
    if (pCurrentState_)
    {
        pCurrentState_->Update(*this, deltaTime);
    }

    /// 移動更新
    if (pCurrentMovement_)
    {
        pCurrentMovement_->Update(transform_, deltaTime);
    }

    /// オブジェクト更新
    if (pObjectSelfBody_)
    {
        pObjectSelfBody_->SetRotate(transform_.rotate);
        pObjectSelfBody_->SetTranslate(transform_.translate);
        pObjectSelfBody_->Update();
    }

    /// コライダー更新
    pSphere_->center_ = transform_.translate;
}

void EnemyRusher::Draw1F()
{
    if (pObjectSelfBody_) pObjectSelfBody_->Draw1F();
}

void EnemyRusher::ImGui()
{
}

void EnemyRusher::OnCollisionTrigger(const Collider* pOther)
{
    bool isCollide = pOther->GetColliderID() == "playerBullet";
    isCollide |= pOther->GetColliderID() == "PlayerExplosion";

    bool isPlayerBullet = pOther->GetColliderID() == "playerBullet";
    bool isPlayerExplosion = pOther->GetColliderID() == "PlayerExplosion";

    /// 衝突している場合
    if (isCollide)
    {
        auto pStatsOther = pOther->GetEntityStats();
        assert(pStatsOther);
        pStats_->OnCollision(pStatsOther);

        if (pStats_->GetHp() <= 0)
        {
            // 死亡する
            EntityBase::Dead();

            /// あたっている相手に応じてスコアイベントを発行
            if (isPlayerBullet)
            {
                EventListener::GetInstance()->Publish(KillEnemyEvent{ EnemyTypes::Rusher, 0.2f });
            }
            else if (isPlayerExplosion)
            {
                EventListener::GetInstance()->Publish(KillEnemyEvent{ EnemyTypes::Rusher, 1.0f });
            }
        }

        /// ヒットパーティクルの再生
        auto pTransformOtherOwner = pOther->GetOwnerTransform();
        assert(pTransformOtherOwner);
        Vector3 hitPos = pTransformOtherOwner->translate;
        if (hitPos.x == 0 && hitPos.y == 0 && hitPos.z == 0)
        {
            assert(0);
        }

        /// 画面揺れ
        EntityBase::ShakeCamera(kCameraShakePower_);
    }
}

void EnemyRusher::OnCollision(const Collider* pOther)
{
    if (pOther->GetColliderID() == "enemy")
    {
        /// 反発を速度に適用
        Vector3 otherPos = pOther->GetOwnerTransform()->translate;
        otherPos.y = transform_.translate.y; // 水平方向のみ反発
        Vector3 dir = transform_.translate - otherPos;
        pFollowMovement_->ApplyForce(dir * kReflectionPower_);
    }
}

void EnemyRusher::ChangeState(std::unique_ptr<EnemyRusherState> newState)
{
    if (pCurrentState_)
    {
        pCurrentState_->Exit(*this);
    }
    pCurrentState_ = std::move(newState);
    if (pCurrentState_)
    {
        pCurrentState_->Enter(*this);
    }
}

void EnemyRusher::FocusOnTarget(float deltaTime)
{
    if (pFocusOrientation_) pFocusOrientation_->Update(transform_, deltaTime);
}

void EnemyRusher::DashToTarget()
{
    if (!pDashMovement_) return;
    if (!params_.pTargetPosition) return;

    Vector3 direction = *(params_.pTargetPosition) - transform_.translate;
    direction.y = 0.0f; // 水平方向のみ
    pDashMovement_->StartDash(direction.Normalized(), kDashDistance_, kDashDurationSec_);
}

void EnemyRusher::ToFollowMovement()
{
    // 現在の移動コンポーネントを追尾移動に設定
    pCurrentMovement_ = pFollowMovement_.get();
}

void EnemyRusher::ToDashMovement()
{
    // 現在の移動コンポーネントをダッシュ移動に設定
    pCurrentMovement_ = pDashMovement_.get();
}

void EnemyRusher::DisableMovement()
{
    pCurrentMovement_ = nullptr;
}

void EnemyRusher::ChangeColor(const Vector4& color)
{
    if (!pObjectSelfBody_) return;
    pObjectSelfBody_->GetOption().materialData->color = color;
}

bool EnemyRusher::IsCloseToTarget(float thresholdDistance) const
{
    if (!params_.pTargetPosition) return false;
    float distance = transform_.translate.Distance(*(params_.pTargetPosition));
    return distance <= thresholdDistance;
}

bool EnemyRusher::IsDashing() const
{
    if (!pDashMovement_) return false;
    return !pDashMovement_->IsFinished();
}

void EnemyRusher::InitializeState()
{
    // 初期状態を追尾状態に設定
    this->ChangeState(std::make_unique<EnemyRusherStateFollow>());
}

void EnemyRusher::InitializeComponents()
{
    this->InitializeTransform();
    this->InitializeBody();
    this->InitializeMovement();
    this->InitializeFocusOrientation();
    this->InitializeStats();
    this->InitializeCollider(pStats_.get());
}

void EnemyRusher::InitializeTransform()
{
    transform_.scale = Vector3(1.2f, 1.2f, 1.2f);
    transform_.rotate = Vector3(0.0f, 0.0f, 0.0f);
    transform_.translate = transform_.scale / 2.0f; // 地面に接地するように調整
}

void EnemyRusher::InitializeBody()
{
    if (!params_.pModelSelfBody) return;
    pObjectSelfBody_ = std::make_unique<Object3d>();
    pObjectSelfBody_->Initialize(false);
    pObjectSelfBody_->SetModel(params_.pModelSelfBody);
    pObjectSelfBody_->SetScale(transform_.scale);
    auto& option = pObjectSelfBody_->GetOption();
    option.materialData->color = RGBA(0x40c4faff).to_Vector4();
    option.lightingData->enableLighting = false;
}

void EnemyRusher::InitializeCollider(EntityStats* pStats)
{
    auto pCollisionManager = CollisionManager::GetInstance();
    if (!params_.pModelSelfBody) return;

    /// コライダー形状の初期化
    pSphere_ = std::make_unique<Sphere>();
    pSphere_->center_ = Vector3(0.0f, 1.0f, 0.0f);
    pSphere_->radius_ = 1.0f;

    /// コライダーの初期化
    pCollider_ = std::make_unique<Collider>();
    pCollider_->SetColliderID("enemy");
    pCollider_->SetAttribute(pCollisionManager->GetNewAttribute("enemy"));
    pCollider_->SetOwner(this);
    pCollider_->SetShape(Shape::Sphere);
    pCollider_->SetShapeData(pSphere_.get());
    pCollider_->SetMask(pCollisionManager->GetNewMask("enemyDummy"));
    pCollider_->SetOnCollisionTrigger(std::bind(&EnemyRusher::OnCollisionTrigger, this, std::placeholders::_1));
    pCollider_->SetOnCollision(std::bind(&EnemyRusher::OnCollision, this, std::placeholders::_1));
    pCollider_->SetOwnerTransform(&transform_);
    pCollider_->SetEntityStats(pStats);
    pCollisionManager->RegisterCollider(pCollider_.get());
}

void EnemyRusher::InitializeMovement()
{
    if (!params_.pTargetPosition) return;
    pFollowMovement_ = std::make_unique<FollowMovement>(params_.pTargetPosition);
    pFollowMovement_->SetFollowSpeed(kFollowSpeed_);
    pDashMovement_ = std::make_unique<DashMovementLinear>();
}

void EnemyRusher::InitializeFocusOrientation()
{
    if (!params_.pTargetPosition) return;
    pFocusOrientation_ = std::make_unique<FocusOrientation>();
    pFocusOrientation_->SetTargetPosition(params_.pTargetPosition);
    pFocusOrientation_->SetRotateRatio(20.0f);
}

void EnemyRusher::InitializeStats()
{
    pStats_ = std::make_unique<EntityStats>();
    pStats_->Initialize(6.0f, 10.0f, 10.0f);
}

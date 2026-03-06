#include "EnemyRusher.h"

#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include "EnemyRusherStateFollow.h"
#include "EnemyRusherStateKnockback.h"
#include <Color.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <functional>
#include <logic/event/KillEnemyEvent.h>
#include <Features/event/EventListener.h>
#include <logic/event/ParticleEmitEvent.h>
#include <presentation/ParticleType.h>
#include <config/ResourcePath.h>
#include <Features/Audio/AudioManager.h>


void EnemyRusher::Initialize(bool enableDebugWindow /*= true*/)
{
    EntityBase::Initialize(enableDebugWindow);
    EntityBase::SetName(utl::debug::generate_name("EnemyRusher", this));

    this->InitializeComponents();
    this->InitializeState();

    /// オーディオの初期化
    audioDeath_ = AudioManager::GetInstance()->GetNewAudio("Effect", Path::Audio::kSeEnemyDeath);
    audioDeath_->SetVolume(0.05f);
    audioAim_ = AudioManager::GetInstance()->GetNewAudio("Effect", Path::Audio::kSeEnemyRusherAim);
    audioAim_->SetVolume(0.05f);
}

void EnemyRusher::Finalize()
{
    EventListener* pEventListener = EventListener::GetInstance();

    ParticleEmitEvent emitEvent;
    emitEvent.position = transform_.translate;

    emitEvent.type = ParticleType::EnemyNormalDeathSpark;
    pEventListener->Publish(emitEvent);
    emitEvent.type = ParticleType::EnemyNormalDeathExplosion;
    pEventListener->Publish(emitEvent);

    CollisionManager::GetInstance()->UnregisterCollider(pCollider_.get());

    pObjectSelfBody_->Finalize();

    if (params_.pDirLight)
    {
        params_.pDirLight->intensity += 0.5f;
        if (params_.pDirLight->intensity > 8.0f)
        {
            params_.pDirLight->intensity = 8.0f;
        }
    }
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
    pPhysicsMovement_->ApplyFriction(0.925f);
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
            audioDeath_->Play();

            /// あたっている相手に応じてスコアイベントを発行
            if (isPlayerBullet)
            {
                EventListener::GetInstance()->Publish(KillEnemyEvent{ EnemyType::Rusher, 0.2f });
            }
            else if (isPlayerExplosion)
            {
                EventListener::GetInstance()->Publish(KillEnemyEvent{ EnemyType::Rusher, 1.0f });
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
    else if (pOther->GetColliderID() == "PlayerExplosion")
    {
        ChangeState(std::make_unique<EnemyRusherStateKnockback>());

        Vector3 otherPos = pOther->GetOwnerTransform()->translate;
        otherPos.y = transform_.translate.y; // 水平方向のみ反発
        Vector3 dir = transform_.translate - otherPos;
        pPhysicsMovement_->ApplyForce(dir * kReflectionPowerPlayerExplosion_);
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
    pFollowMovement_->ResetVelocity();
}

void EnemyRusher::ToDashMovement()
{
    // 現在の移動コンポーネントをダッシュ移動に設定
    pCurrentMovement_ = pDashMovement_.get();
}

void EnemyRusher::ToPhysicsMovement()
{
    pCurrentMovement_ = pPhysicsMovement_.get();
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

bool EnemyRusher::IsStopped() const
{
    if (!pCurrentMovement_) return true;

    // 追尾移動や物理移動の場合は、速度がほぼ0かどうかで判定
    Vector3 vel = pPhysicsMovement_->GetVelocity();
    Vector3 acc = pPhysicsMovement_->GetAcceleration();
    bool isPhysicsStopped = vel.Length() < 0.01f && acc.Length() < 0.01f;
    if (isPhysicsStopped)
    {
        return true;
    }

    return false; // 追尾移動や物理移動は常に停止状態ではないとみなす
}

float EnemyRusher::GetDashElapsedTime() const
{
    if (!pDashMovement_) return 0.0f;
    return pDashMovement_->GetElapsedTime();
}

void EnemyRusher::PlaySoundOnAim() const
{
    if (audioAim_)
    {
        audioAim_->Play();
    }
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
    transform_.translate.x = params_.position.x;
    transform_.translate.z = params_.position.z;
}

void EnemyRusher::InitializeBody()
{
    if (!params_.pModelSelfBody) return;
    pObjectSelfBody_ = std::make_unique<Object3d>();
    pObjectSelfBody_->Initialize(false);
    pObjectSelfBody_->SetModel(params_.pModelSelfBody);
    pObjectSelfBody_->SetScale(transform_.scale);
    auto& option = pObjectSelfBody_->GetOption();
    option.materialData->color = kColorDefault_.to_Vector4();
    option.lightingData->enableLighting = false;
    option.materialData->environmentCoefficient = 0.0f;
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
    pPhysicsMovement_ = std::make_unique<PhysicsMovement>();
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
    pStats_->Initialize(12.0f, 10.0f, 10.0f);
}

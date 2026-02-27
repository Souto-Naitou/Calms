#include "EnemyNormal.h"

#include <imgui.h>
#include <Utility/Debug/dbgutl.h>
#include <config/ResourcePath.h>
#include <Features/Event/EventListener.h>
#include <logic/event/KillEnemyEvent.h>
#include <logic/event/ParticleEmitEvent.h>

EnemyNormal::EnemyNormal(const EnemyNormalInitParams& param)
{
    pModelSelfBody_ = param.pModelSelfBody->Cloned();
    params_ = param;
}

void EnemyNormal::Initialize(bool enableDebugWindow)
{
    /// 基底クラスの初期化
    EntityBase::Initialize(enableDebugWindow);
    EntityBase::SetName(utl::debug::generate_name("Enemy", this));

    /// インスタンスの取得
    pCollisionManager_  = CollisionManager::GetInstance();
    pDeltaTimeManager_  = DeltaTimeManager::GetInstance();
    EntityBase::SetGameEye(Object3dSystem::GetInstance()->GetGlobalEye());

    /// パラメータの初期化
    transform_.translate = params_.position;
    pStats_ = std::make_unique<EntityStats>();
    pStats_->Initialize(1.0f, 10.0f, 10.0f);

    // オブジェクトの初期化
    this->InitializeObjects();

    // コライダーの初期化
    this->InitializeCollider();

    sphereLine_.Initialize();

    this->InitializeComponents();

    /// オーディオの初期化
    audioDeath_ = AudioManager::GetInstance()->GetNewAudio("Effect", Path::Audio::kSeEnemyDeath);
    audioDeath_->SetVolume(0.05f);
}

void EnemyNormal::Finalize()
{
    /// コライダーの削除
    pCollisionManager_->UnregisterCollider(pCollider_.get());

    pObjectSelfBody_->Finalize();

    EventListener* pEventListener = EventListener::GetInstance();

    ParticleEmitEvent emitEvent;
    emitEvent.position = transform_.translate;

    emitEvent.type = ParticleType::EnemyNormalDeathSpark;
    pEventListener->Publish(emitEvent);
    emitEvent.type = ParticleType::EnemyNormalDeathExplosion;
    pEventListener->Publish(emitEvent);

    if (params_.pDirLight) 
    {
        params_.pDirLight->intensity += 0.5f;
        if (params_.pDirLight->intensity > 8.0f)
        {
            params_.pDirLight->intensity = 8.0f;
        }
    }
}

void EnemyNormal::Update()
{
    const auto  dtChannel = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    const float deltaTime = pDeltaTimeManager_->GetDeltaTime(dtChannel);

    /// オブジェクトの更新
    this->UpdateObjects();

    /// コライダーの更新
    this->UpdateCollider();

    /// コンポーネントの更新
    pMovement_->ApplyFriction(kFriction_);
    pMovement_->Update(transform_, deltaTime);
    pFocusOrientation_->Update(transform_, deltaTime);
}

void EnemyNormal::Draw1F()
{
    if (pObjectSelfBody_) pObjectSelfBody_->Draw1F();
    if (isDrawCollisionArea_) sphereLine_.Draw1F();
}

void EnemyNormal::InitializeComponents()
{
    pMovement_ = std::make_unique<FollowMovement>(params_.pTargetPosition);
    pMovement_->SetFollowSpeed(kFollowSpeed_);
    pFocusOrientation_ = std::make_unique<FocusOrientation>();
    pFocusOrientation_->SetTargetPosition(params_.pTargetPosition);
    pFocusOrientation_->SetRotateRatio(0.95f);
}

void EnemyNormal::InitializeObjects()
{
    if (pModelSelfBody_ == nullptr)
    {
        Logger::GetInstance()->LogError(__FILE__, __FUNCTION__, "pModelSelfBody_ がnullptrです");
    }

    /// オブジェクトの初期化
    pObjectSelfBody_ = std::make_unique<Object3d>();
    pObjectSelfBody_->Initialize(false);
    pObjectSelfBody_->SetName("enemy");
    pObjectSelfBody_->SetTranslate(Vector3(0, 0.5f, 0));
    pObjectSelfBody_->SetRotate(Vector3(0, 0, 0));
    pObjectSelfBody_->SetModel(pModelSelfBody_.get());
    auto& option = pObjectSelfBody_->GetOption();
    option.materialData->environmentCoefficient = 0.0f;
    option.materialData->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    option.lightingData->enableLighting = false;
}

void EnemyNormal::InitializeCollider()
{
    /// コライダーの初期化
    pCollider_ = std::make_unique<Collider>();
    pCollider_->SetColliderID("enemy");
    pCollider_->SetAttribute(pCollisionManager_->GetNewAttribute("enemy"));
    pCollider_->SetOwner(this);
    pCollider_->SetShape(Shape::Sphere);
    pCollider_->SetShapeData(&sphere_);
    pCollider_->SetMask(pCollisionManager_->GetNewMask("enemyDummy"));
    pCollider_->SetOnCollisionTrigger(std::bind(&EnemyNormal::OnCollisionTrigger, this, std::placeholders::_1));
    pCollider_->SetOnCollision(std::bind(&EnemyNormal::OnCollision, this, std::placeholders::_1));
    pCollider_->SetOwnerTransform(&transform_);
    pCollider_->SetEntityStats(pStats_.get());
    pCollisionManager_->RegisterCollider(pCollider_.get());
}

void EnemyNormal::UpdateCollider()
{
    /// コライダーの更新
    sphere_.center_ = transform_.translate;
    sphere_.radius_ = 0.75f;
    sphereLine_.SetTransform({ Vector3(1,1,1), Vector3(0,0,0), transform_.translate });
    sphereLine_.Update();

    pCollider_->SetShapeData(&sphere_);
}

void EnemyNormal::UpdateObjects()
{
    pObjectSelfBody_->SetTranslate(transform_.translate);
    pObjectSelfBody_->SetRotate(transform_.rotate);
    pObjectSelfBody_->Update();
}

void EnemyNormal::OnCollision(const Collider* other)
{
    if (other->GetColliderID() == "enemy")
    {
        /// 反発を速度に適用
        Vector3 otherPos = other->GetOwnerTransform()->translate;
        Vector3 dir = transform_.translate - otherPos;
        pMovement_->ApplyForce(dir * kReflectionPower_);
    }
}

void EnemyNormal::OnCollisionTrigger(const Collider* other)
{
    bool isCollide = other->GetColliderID() == "playerBullet";
    isCollide |= other->GetColliderID() == "PlayerExplosion";

    bool isPlayerBullet = other->GetColliderID() == "playerBullet";
    bool isPlayerExplosion = other->GetColliderID() == "PlayerExplosion";

    /// 衝突している場合
    if (isCollide)
    {
        auto pStatsOther = other->GetEntityStats();
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
                EventListener::GetInstance()->Publish(KillEnemyEvent{ EnemyType::Normal, 0.2f });
            }
            else if (isPlayerExplosion)
            {
                EventListener::GetInstance()->Publish(KillEnemyEvent{ EnemyType::Normal, 1.0f });
            }
        }

        /// ヒットパーティクルの再生
        auto pTransformOtherOwner = other->GetOwnerTransform();
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

void EnemyNormal::ImGui()
{
#ifdef _DEBUG
    EntityBase::ImGui();
    ImGui::Checkbox("Draw2D Collision Area", &isDrawCollisionArea_);
#endif
}
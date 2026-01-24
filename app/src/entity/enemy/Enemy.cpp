#include "Enemy.h"

#include <imgui.h>
#include <Utility/Debug/dbgutl.h>
#include <config/ResourcePath.h>
#include <Features/Event/EventListener.h>
#include <logic/event/KillEnemyEvent.h>

Enemy::Enemy(const Params& params)
{
    pModelSelfBody_ = params.pModelSelfBody->Cloned();
    params_ = params;
}

void Enemy::Initialize(const EntityCommonParams& params, bool enableDebugWindow)
{
    /// 基底クラスの初期化
    EntityBase::Initialize(params, enableDebugWindow);
    if (isEnableDebugWindow_)
    {
        pDebugEntry_->SetName(utl::debug::generate_name("Enemy", this));
    }

    /// インスタンスの取得
    collisionManager_ = CollisionManager::GetInstance();
    deltaTimeManager_ = DeltaTimeManager::GetInstance();
    ppGameEye_        = Object3dSystem::GetInstance()->GetGlobalEye();

    /// パラメータの初期化
    friction_               = 0.95f;
    moveSpeed_              = 10.0f;
    transform_.translate    = Vector3(0, 0.5f, 0);
    attackPower_            = 10.0f;
    stats_.Initialize(1.0f, 10.0f, 10.0f);

    // オブジェクトの初期化
    this->InitializeObjects();

    // コライダーの初期化
    this->InitializeCollider();

    sphereLine_.Initialize();

    // コライダーの登録
    collisionManager_->RegisterCollider(collider_.get());

    // パーティクルエミッターの初期化
    this->InitializeParticleEmitters();

    /// オーディオの初期化
    audioDeath_ = AudioManager::GetInstance()->GetNewAudio("Effect", Path::Audio::kSeEnemyDeath);
    audioDeath_->SetVolume(0.05f);

    if (params.pDirLight) objectSelfBody_->SetDirectionalLight(params.pDirLight);
    if (params.pPointLight) objectSelfBody_->SetPointLight(params.pPointLight);
}

void Enemy::Finalize()
{
    /// コライダーの削除
    collisionManager_->DeleteCollider(collider_.get());

    objectSelfBody_->Finalize();

    pParticleDeathShort_->SetPosition(transform_.translate);
    pParticleDeathShort_->Emit();
    pParticleDeathSplatter_->SetPosition(transform_.translate);
    pParticleDeathSplatter_->Emit();

    pParticleDeathShort_->Finalize();
    pParticleDeathSplatter_->Finalize();

    if (commonParams_.pDirLight) 
    {
        commonParams_.pDirLight->intensity += 0.5f;
        if (commonParams_.pDirLight->intensity > 8.0f)
        {
            commonParams_.pDirLight->intensity = 8.0f;
        }
    }
}

void Enemy::Update()
{
    // 変形情報の更新 (プレイヤーに向かって追尾・向き変更)
    this->UpdateTransform();

    // 物理演算の更新
    EntityBase::UpdatePhysics(deltaTimeManager_->GetDeltaTime(1));

    // オブジェクトの更新
    this->UpdateObjects();

    // コライダーの更新
    this->UpdateCollider();

    // パーティクルの更新
    pParticleDeathShort_->Update();
    pParticleDeathSplatter_->Update();
}

void Enemy::Draw1F()
{
    if (objectSelfBody_) objectSelfBody_->Draw1F();
    if (isDrawCollisionArea_) sphereLine_.Draw1F();
}

void Enemy::InitializeObjects()
{
    if (pModelSelfBody_ == nullptr)
    {
        Logger::GetInstance()->LogError(__FILE__, __FUNCTION__, "pModelSelfBody_ がnullptrです");
    }

    /// オブジェクトの初期化
    objectSelfBody_ = std::make_unique<Object3d>();
    objectSelfBody_->Initialize(false);
    objectSelfBody_->SetName("enemy");
    objectSelfBody_->SetTranslate(Vector3(0, 0.5f, 0));
    objectSelfBody_->SetRotate(Vector3(0, 0, 0));
    objectSelfBody_->SetModel(pModelSelfBody_.get());
    auto& option = objectSelfBody_->GetOption();
    option.materialData->environmentCoefficient = 0.0f;
    option.materialData->color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
    option.lightingData->enableLighting = false;
}

void Enemy::InitializeCollider()
{
    /// コライダーの初期化
    collider_ = std::make_unique<Collider>();
    collider_->SetColliderID("enemy");
    collider_->SetAttribute(collisionManager_->GetNewAttribute("enemy"));
    collider_->SetOwner(this);
    collider_->SetShape(Shape::Sphere);
    collider_->SetShapeData(&sphere_);
    collider_->SetMask(collisionManager_->GetNewMask("enemyDummy"));
    collider_->SetOnCollisionTrigger(std::bind(&Enemy::OnCollisionTrigger, this, std::placeholders::_1));
    collider_->SetOnCollision(std::bind(&Enemy::OnCollision, this, std::placeholders::_1));
    collider_->SetEnableLighter(false);
}

void Enemy::InitializeParticleEmitters()
{
    /// パラメータを作成
    ParticleEmitterInitParams params;

    params.particle = params_.pParticleTriangle;
    params.jsonPath = "resources/json/particles/Death.json";
    pParticleDeathShort_ = std::make_unique<ParticleEmitter>();
    pParticleDeathShort_->Initialize(params);
    pParticleDeathShort_->SetEnableBillboard(true);
    pParticleDeathShort_->SetPosition(transform_.translate);
    pParticleDeathShort_->EnableManualMode();
    
    params.particle = params_.pParticleCircle;
    params.jsonPath = "resources/json/particles/Death2.json";
    pParticleDeathSplatter_ = std::make_unique<ParticleEmitter>();
    pParticleDeathSplatter_->Initialize(params);
    pParticleDeathSplatter_->SetEnableBillboard(true);
    pParticleDeathSplatter_->SetPosition(transform_.translate);
    pParticleDeathSplatter_->EnableManualMode();
}

void Enemy::UpdateTransform()
{
    if (!locationProvider_) return;

    positionTarget_ = locationProvider_->GetTranslation().xz();
    distanceToTarget = positionTarget_ - transform_.translate.xz();

    /// 追尾
    if (distanceToTarget.Length() > 0)
    {
        Vector2 normalDist2Target = distanceToTarget.Normalize();
        velocity_move = normalDist2Target * moveSpeed_;
        acceleration_ = Vector3(velocity_move.x, 0, velocity_move.y);
    }

    acceleration_ += accelerationRefl_;
    accelerationRefl_ = Vector3(0, 0, 0);

    /// 方向を変更
    if ((distanceToTarget.x != 0 || distanceToTarget.y != 0))
    {
        transform_.rotate = Vector3(0, -velocity_.xz().Theta(), 0);
    }
}

void Enemy::UpdateCollider()
{
    /// コライダーの更新
    sphere_.center_ = transform_.translate;
    sphere_.radius_ = 0.75f;
    sphereLine_.SetTransform({ Vector3(1,1,1), Vector3(0,0,0), transform_.translate });
    sphereLine_.Update();

    collider_->SetShapeData(&sphere_);
}

void Enemy::UpdateObjects()
{
    objectSelfBody_->SetTranslate(transform_.translate);
    objectSelfBody_->SetRotate(transform_.rotate);
    objectSelfBody_->Update();
}

void Enemy::OnCollision(const Collider* _other)
{
    if (_other->GetColliderID() == "enemy")
    {
        const EntityBase* otherOwner = _other->GetOwner<EntityBase>();

        /// 反発を速度に適用
        Vector3 otherPos = otherOwner->GetTranslation();
        Vector3 dir = transform_.translate - otherPos;
        accelerationRefl_ = dir * reflectionPower_;
    }
}

void Enemy::OnCollisionTrigger(const Collider* other)
{
    bool isCollide = other->GetColliderID() == "playerBullet";
    isCollide |= other->GetColliderID() == "PlayerExplosion";

    bool isPlayerBullet = other->GetColliderID() == "playerBullet";
    bool isPlayerExplosion = other->GetColliderID() == "PlayerExplosion";

    /// 衝突している場合
    if (isCollide)
    {
        const EntityBase* otherOwner = other->GetOwner<EntityBase>();
        stats_.OnCollision(otherOwner->GetStats());
        
        if (stats_.GetHp() <= 0) 
        {
            isAlive_ = false;
            audioDeath_->Play();
            
            /// あたっている相手に応じてスコアイベントを発行
            if (isPlayerBullet)
            {
                EventListener::GetInstance()->Publish(KillEnemyEvent{ EnemyTypes::Normal, 0.2f });
            }
            else if (isPlayerExplosion)
            {
                EventListener::GetInstance()->Publish(KillEnemyEvent{ EnemyTypes::Normal, 1.0f });
            }
        }

        /// ヒットパーティクルの再生
        Vector3 hitPos = otherOwner->GetTranslation();
        if (hitPos.x == 0 && hitPos.y == 0 && hitPos.z == 0)
        {
            assert(0);
        }

        /// 反発を速度に適用
        Vector3 dir = transform_.translate - hitPos;
        accelerationRefl_ = dir * bulletReflectionPower_;

        /// 画面揺れ
        (*ppGameEye_)->Shake(0.1f);
    }
}

void Enemy::ImGui()
{
#ifdef _DEBUG
    EntityBase::ImGui();
    ImGui::Checkbox("Draw2D Collision Area", &isDrawCollisionArea_);
#endif
}

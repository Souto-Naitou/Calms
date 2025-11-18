#include "Enemy.h"

#include <imgui.h>
#include <Utility/Debug/dbgutl.h>
#include <config/ResourcePath.h>

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
    stats_.Initalize(1.0f, 10.0f, 10.0f);

    // オブジェクトの初期化
    this->InitializeObjects();

    // コライダーの初期化
    this->InitializeCollider();

    // OBBの初期化
    obb_.Initialize();

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

    pParticleDeath_->SetPosition(transform_.translate);
    pParticleDeath_->Emit();

    pParticleDeath_->Finalize();

    commonParams_.pDirLight->intensity += 0.5f;

    EntityBase::Finalize();
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
    pParticleDeath_->Update();
}

void Enemy::Draw1F()
{
    if (objectSelfBody_) objectSelfBody_->Draw1F();
}

void Enemy::DrawLine()
{
    if (isDrawCollisionArea_) collider_->DrawArea();
    pParticleDeath_->Draw();
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
    collider_->SetShape(Shape::OBB);
    collider_->SetShapeData(&obb_);
    collider_->SetRadius(2);
    collider_->SetMask(collisionManager_->GetNewMask("enemyDummy"));
    collider_->SetOnCollisionTrigger(std::bind(&Enemy::OnCollisionTrigger, this, std::placeholders::_1));
    collider_->SetOnCollision(std::bind(&Enemy::OnCollision, this, std::placeholders::_1));
    collider_->SetEnableLighter(true);
}

void Enemy::InitializeParticleEmitters()
{
    /// パラメータを作成
    ParticleEmitterInitParams params;

    params.particle = params_.pParticleDeath;
    params.jsonPath = "resources/json/particles/Death.json";
    pParticleDeath_ = std::make_unique<ParticleEmitter>();
    pParticleDeath_->Initialize(params);
    pParticleDeath_->SetEnableBillboard(true);
    pParticleDeath_->SetPosition(transform_.translate);
    pParticleDeath_->EnableManualMode();
}

void Enemy::UpdateTransform()
{
    if (locationProvider_) positionTarget_ = locationProvider_->GetTranslation().xz();
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
    obb_.SetCenter(transform_.translate);
    obb_.SetOrientations(objectSelfBody_->GetRotateMatrix());
    obb_.SetSize(Vector3(0.5f, 0.5f, 0.5f));

    collider_->SetShapeData(&obb_);
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

void Enemy::OnCollisionTrigger(const Collider* _other)
{
    if (_other->GetColliderID() == "playerBullet")
    {
        const EntityBase* otherOwner = _other->GetOwner<EntityBase>();

        stats_.OnCollision(otherOwner->GetStats());
        
        if (stats_.GetHp() <= 0) 
        {
            isAlive_ = false;
            audioDeath_->Play();
        }

        /// ヒットパーティクルの再生
        Vector3 hitPos = otherOwner->GetTranslation();

        if (hitPos.x == 0 && hitPos.y == 0 && hitPos.z == 0)
        {
            assert(0);
        }

        Vector3 dir = transform_.translate - hitPos;

        accelerationRefl_ = dir * bulletReflectionPower_;

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

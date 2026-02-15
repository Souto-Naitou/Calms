#include "PlayerBullet.h"

#include <imgui.h>

PlayerBullet::PlayerBullet(const Params& param)
{
    params_ = param;
}

void PlayerBullet::Initialize(bool enableDebugWindow)
{
    EntityBase::Initialize(enableDebugWindow);
    EntityBase::SetName(utl::debug::generate_name("PlayerBullet", this));

    /// インスタンスの取得
    collisionManager_ = CollisionManager::GetInstance();
    deltaTimeManager_ = DeltaTimeManager::GetInstance();

    /// タイマーの初期化
    pTimeMeasurer_ = std::make_unique<TimeMeasurer>();
    pTimeMeasurer_->Start();

    // コンポーネントの初期化
    this->ComponentsInitialize();

    // オブジェクトの初期化
    this->ObjectsInitialize();

    // コライダーの初期化
    this->CollidersInitialize();
}


void PlayerBullet::Finalize()
{
    params_.pParticleData->currentColor = {};
    params_.pParticleData->colorRange = {};

    collisionManager_->UnregisterCollider(pCollider_.get());
}


void PlayerBullet::Update()
{
    const uint32_t  deltaTimeChannel    = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    const float     deltaTime           = deltaTimeManager_->GetDeltaTime(deltaTimeChannel);

    // 生存フラグの更新
    if (pTimeMeasurer_->GetNow<float>() > kLifeTimeLimit_)
    {
        EntityBase::Dead();
    }

    // 位置の更新
    pMovement_->AddImpulse(params_.direction_ * params_.moveSpeed_);
    pMovement_->Update(transform_, deltaTime);
    pMovement_->ResetVelocity();

    // オブジェクトの更新
    this->ObjectsUpdate();

    // OBBの更新
    sphere_.center_ = transform_.translate;
    sphere_.radius_ = 0.3f;

    pCollider_->SetShapeData(&sphere_);
}


void PlayerBullet::Draw1F()
{
}

void PlayerBullet::OnCollisionTrigger(const Collider* _other)
{
    if (_other->GetColliderID() == "enemy")
    {
        EntityBase::Dead();
    }
}

void PlayerBullet::ImGui()
{
#ifdef _DEBUG
    EntityBase::ImGui();
    ImGui::Checkbox("Draw2D Collision Area", &isDrawCollisionArea_);
#endif
}

void PlayerBullet::ObjectsInitialize()
{
    /// [ オブジェクトの初期化 ]
    /// - パーティクルを使用して弾を描画する
    auto& data = params_.pParticleData;
    data->transform.translate = {0.0f, 0.5f, 0.0f};
    data->colorRange = Range(RGBA(0xffffffff).to_Vector4(), RGBA(0x91bbffff).to_Vector4());
    data->transform.scale = { 0.1f, 0.1f, 0.1f };
    data->scaleRange = Range<Vector3>({ 0.1f, 0.1f, 0.1f }, { 0.1f, 0.1f, 0.1f });
    data->deleteCondition = ParticleDeleteCondition::ZeroAlpha;
}

void PlayerBullet::ObjectsUpdate()
{
    // 位置の反映
    auto& data = params_.pParticleData;
    data->transform.translate = transform_.translate;
}

void PlayerBullet::CollidersInitialize()
{
    /// コライダーの初期化
    pCollider_ = std::make_unique<Collider>(false);
    pCollider_->SetColliderID("playerBullet");
    pCollider_->SetAttribute(collisionManager_->GetNewAttribute("playerBullet"));
    pCollider_->SetOwner(this);
    pCollider_->SetShape(Shape::Sphere);
    pCollider_->SetMask(collisionManager_->GetNewMask("playerBullet", "player"));
    pCollider_->SetOnCollisionTrigger(std::bind(&PlayerBullet::OnCollisionTrigger, this, std::placeholders::_1));
    pCollider_->SetOwnerTransform(&transform_);
    pCollider_->SetEntityStats(pStats_.get());

    collisionManager_->RegisterCollider(pCollider_.get());
}

void PlayerBullet::ComponentsInitialize()
{
    transform_.translate = params_.initPosition_;
    pMovement_ = std::make_unique<PhysicsMovement>();
    pStats_ = std::make_unique<EntityStats>();
    pStats_->Initialize(1.0f, 1.0f, 1.0f);
}

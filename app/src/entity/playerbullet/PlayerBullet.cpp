#include "PlayerBullet.h"

#include <imgui.h>

PlayerBullet::PlayerBullet(const Params& param)
{
    params_ = param;
}

void PlayerBullet::Initialize(const EntityCommonParams& params, bool enableDebugWindow)
{
    EntityBase::Initialize(params, enableDebugWindow);
    if (isEnableDebugWindow_)
    {
        pDebugEntry_->SetName(utl::debug::generate_name("PlayerBullet", this));
    }

    /// インスタンスの取得
    collisionManager_ = CollisionManager::GetInstance();
    deltaTimeManager_ = DeltaTimeManager::GetInstance();

    /// タイマーの初期化
    timer_ = std::make_unique<TimeMeasurer>();
    timer_->Start();

    // オブジェクトの初期化
    this->ObjectsInitialize();

    /// パラメータの初期化
    friction_ = 1.0f;
    attackPower_ = 5.0f;
    stats_.Initalize(1.0f, 5.0f, 1.0f);

    // コライダーの初期化
    this->CollidersInitialize();
}


void PlayerBullet::Finalize()
{
    params_.particleData->currentColor = {};
    params_.particleData->colorRange = {};

    collisionManager_->DeleteCollider(collider_.get());

    EntityBase::Finalize();
}


void PlayerBullet::Update()
{
    // 生存フラグの更新
    if (timer_->GetNow<float>() > lifeTimeLimit_)
    {
        isAlive_ = false;
    }

    // 速度の決定
    velocity_ = moveVelocity_;

    // 位置の更新
    EntityBase::UpdatePhysics(deltaTimeManager_->GetDeltaTime(1));

    // オブジェクトの更新
    this->ObjectsUpdate();

    // OBBの更新
    sphere_.center = transform_.translate;
    sphere_.radius = 0.3f;

    collider_->SetShapeData(&sphere_);
}


void PlayerBullet::Draw1F()
{
}

void PlayerBullet::DrawLine()
{
    if (isDrawCollisionArea_) collider_->DrawArea();
}

void PlayerBullet::OnCollisionTrigger(const Collider* _other)
{
    if (_other->GetColliderID() == "enemy")
    {
        isAlive_ = false;
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
    auto& data = params_.particleData;
    data->transform.translate = {0.0f, 0.5f, 0.0f};
    data->colorRange = Range(RGBA(0xffffffff).to_Vector4(), RGBA(0x91bbffff).to_Vector4());
    data->transform.scale = { 0.1f, 0.1f, 0.1f };
    data->scaleRange = Range<Vector3>({ 0.1f, 0.1f, 0.1f }, { 0.1f, 0.1f, 0.1f });
    data->deleteCondition = ParticleDeleteCondition::ZeroAlpha;
}

void PlayerBullet::ObjectsUpdate()
{
    // 位置の反映
    auto& data = params_.particleData;
    data->transform.translate = transform_.translate;
}

void PlayerBullet::CollidersInitialize()
{
    /// コライダーの初期化
    collider_ = std::make_unique<Collider>(false);
    collider_->SetColliderID("playerBullet");
    collider_->SetAttribute(collisionManager_->GetNewAttribute("playerBullet"));
    collider_->SetOwner(this);
    collider_->SetShape(Shape::Sphere);
    collider_->SetRadius(1);
    collider_->SetMask(collisionManager_->GetNewMask("playerBullet", "player"));
    collider_->SetOnCollisionTrigger(std::bind(&PlayerBullet::OnCollisionTrigger, this, std::placeholders::_1));
    collider_->SetEnableLighter(true);

    collisionManager_->RegisterCollider(collider_.get());
}
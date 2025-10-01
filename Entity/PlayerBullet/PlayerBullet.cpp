#include "PlayerBullet.h"

#include <imgui.h>

void PlayerBullet::Initialize(bool _enableDebugWindow)
{
    EntityBase::Initialize(_enableDebugWindow);
    pDebugEntry_->SetName("PlayerBullet");

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

    // OBBの初期化
    obb_.Initialize();

    // コライダーの初期化
    this->CollidersInitialize();
}


void PlayerBullet::Finalize()
{
    pObjectSelfBody_->Finalize();
    pObjectSelfBody_.reset();

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
    obb_.SetCenter(translation_);
    obb_.SetOrientations(pObjectSelfBody_->GetRotateMatrix());
    obb_.SetSize(Vector3(0.3f, 0.3f, 0.3f));

    collider_->SetShapeData(&obb_);
}


void PlayerBullet::Draw()
{
    pObjectSelfBody_->Draw();
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
    /// オブジェクトの初期化
    pObjectSelfBody_ = std::make_unique<Object3d>();
    pObjectSelfBody_->Initialize(false);
    pObjectSelfBody_->SetScale(Vector3(0.3f, 0.3f, 0.3f));
    pObjectSelfBody_->SetTranslate(Vector3(0, 0.5f, 0));
    pObjectSelfBody_->SetRotate(Vector3(0, 0, 0));
    pObjectSelfBody_->SetModel(pModelSelfBody_);
    pObjectSelfBody_->GetOption().materialData->environmentCoefficient = 0.0f;
}

void PlayerBullet::ObjectsUpdate()
{
    // 位置の反映
    pObjectSelfBody_->SetTranslate(translation_);

    // オブジェクトの更新
    if (!directionalLight_)
    {
        directionalLight_ = diContainer_->Resolve<DirectionalLight>();
        pObjectSelfBody_->SetDirectionalLight(directionalLight_);
    }

    if (!pointLight_)
    {
        pointLight_ = diContainer_->Resolve<PointLight>();
        pObjectSelfBody_->SetPointLight(pointLight_);
    }

    pObjectSelfBody_->Update();
}

void PlayerBullet::CollidersInitialize()
{
    /// コライダーの初期化
    collider_ = std::make_unique<Collider>(false);
    collider_->SetColliderID("playerBullet");
    collider_->SetAttribute(collisionManager_->GetNewAttribute("playerBullet"));
    collider_->SetOwner(this);
    collider_->SetShape(Shape::OBB);
    collider_->SetRadius(1);
    collider_->SetMask(collisionManager_->GetNewMask("playerBullet", "player"));
    collider_->SetOnCollisionTrigger(std::bind(&PlayerBullet::OnCollisionTrigger, this, std::placeholders::_1));
    collider_->SetEnableLighter(true);

    collisionManager_->RegisterCollider(collider_.get());
}
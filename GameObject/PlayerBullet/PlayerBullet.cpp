#include "PlayerBullet.h"

#include <imgui.h>

void PlayerBullet::Initialize()
{
    BaseObject::Initialize();

    collisionManager_ = CollisionManager::GetInstance();

    name_ = "playerBullet";

    /// タイマーの初期化
    timer_ = std::make_unique<Timer>();
    timer_->Start();

    /// オブジェクトの初期化
    object_ = std::make_unique<Object3d>();
    object_->Initialize("Cube.obj");
    object_->SetScale(Vector3(0.3f, 0.3f, 0.3f));
    object_->SetTranslate(Vector3(0, 0.5f, 0));
    object_->SetRotate(Vector3(0, 0, 0));


    /// OBBの初期化
    obb_.Initialize();


    /// コライダーの初期化
    collider_ = std::make_unique<Collider>();
    collider_->SetColliderID("playerBullet");
    collider_->SetAttribute(collisionManager_->GetNewAttribute("playerBullet"));
    collider_->SetOwner(this);
    collider_->SetShape(Shape::OBB);
    collider_->SetRadius(2u);
    collider_->SetMask(collisionManager_->GetNewMask("playerBullet", "player"));

    collisionManager_->RegisterCollider(collider_.get());
}


void PlayerBullet::Finalize()
{
    object_->Finalize();
    object_.reset();

    collisionManager_->DeleteCollider(collider_.get());

    BaseObject::Finalize();
}


void PlayerBullet::Update()
{
    // 生存フラグの更新
    if (timer_->GetNow() > lifeTimeLimit_)
    {
        isAlive_ = false;
    }

    // 速度の決定
    velocity_ = moveVelocity_;

    // 位置の更新
    BaseObject::UpdateTransform();

    // 位置の反映
    object_->SetTranslate(translation_);

    // オブジェクトの更新
    if (!directionalLight_)
    {
        directionalLight_ = diContainer_->Resolve<DirectionalLight>();
        object_->SetDirectionalLight(directionalLight_);
    }

    if (!pointLight_)
    {
        pointLight_ = diContainer_->Resolve<PointLight>();
        object_->SetPointLight(pointLight_);
    }

    object_->Update();


    obb_.SetCenter(translation_);
    obb_.SetOrientations(object_->GetRotateMatrix());
    obb_.SetSize(Vector3(0.3f, 0.3f, 0.3f));

    collider_->SetShapeData(&obb_);
}


void PlayerBullet::Draw()
{
    object_->Draw();
}

void PlayerBullet::DrawLine()
{
    if (isDrawCollisionArea_) collider_->DrawArea();
}


void PlayerBullet::ModifyGameEye(GameEye* _eye)
{
    object_->SetGameEye(_eye);
    obb_.SetGameEye(gameEye_);
}

void PlayerBullet::DebugWindow()
{
    BaseObject::DebugWindow();
    ImGui::Checkbox("Draw2D Collision Area", &isDrawCollisionArea_);
}

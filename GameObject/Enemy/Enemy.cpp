#include "Enemy.h"

#include <imgui.h>

void Enemy::Initialize()
{
    /// 基底クラスの初期化
    BaseObject::Initialize();


    /// インスタンスの取得
    collisionManager_ = CollisionManager::GetInstance();


    /// パラメータの初期化
    name_ = "enemy";

    std::stringstream ss;
    ss << "##0x" << std::hex << this;
    name_ += ss.str();

    friction_ = 0.95f;
    moveSpeed_ = 10.0f;
    translation_ = Vector3(0, 0.5f, 0);


    /// オブジェクトの初期化
    object_ = std::make_unique<Object3d>();
    object_->Initialize("Cube.obj");
    object_->SetName("enemy");
    object_->SetTranslate(Vector3(0, 0.5f, 0));
    object_->SetRotate(Vector3(0, 0, 0));


    /// コライダーの初期化
    collider_ = std::make_unique<Collider>();
    collider_->SetColliderID("enemy");
    collider_->SetAttribute(collisionManager_->GetNewAttribute("enemy"));
    collider_->SetOwner(this);
    collider_->SetShape(Shape::OBB);
    collider_->SetShapeData(&obb_);
    collider_->SetRadius(2u);
    collider_->SetMask(collisionManager_->GetNewMask("enemyDummy"));
    collider_->SetOnCollisionTrigger(std::bind(&Enemy::OnCollisionTrigger, this, std::placeholders::_1));
    collider_->SetOnCollision(std::bind(&Enemy::OnCollision, this, std::placeholders::_1));


    /// OBBの初期化
    obb_.Initialize();

    // コライダーの登録
    collisionManager_->RegisterCollider(collider_.get());
}

void Enemy::Finalize()
{
    /// コライダーの削除
    collisionManager_->DeleteCollider(collider_.get());

    object_->Finalize();
    object_.reset();
    BaseObject::Finalize();
}

void Enemy::Update()
{
    if (locationProvider_) positionTarget_ = locationProvider_->GetTranslation().xz();
    distanceToTarget = positionTarget_ - translation_.xz();

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
        rotation_ = Vector3(0, -velocity_.xz().Theta(), 0);
    }

    BaseObject::UpdateTransform();

    object_->SetTranslate(translation_);
    object_->SetRotate(rotation_);

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

    /// コライダーの更新
    obb_.SetCenter(translation_);
    obb_.SetOrientations(object_->GetRotateMatrix());
    obb_.SetSize(Vector3(0.5f, 0.5f, 0.5f));

    collider_->SetShapeData(&obb_);
}

void Enemy::Draw()
{
    if (object_) object_->Draw();
}

void Enemy::DrawLine()
{
    if (isDrawCollisionArea_) collider_->DrawArea();
}

void Enemy::ModifyGameEye(GameEye* _eye)
{
    if (object_) object_->SetGameEye(_eye);
    obb_.SetGameEye(gameEye_);
}

void Enemy::OnCollision(const Collider* _other)
{
    if (_other->GetColliderID() == "enemy")
    {
        /// 反発を速度に適用
        Vector3 otherPos = _other->GetOwner()->GetTranslation();
        Vector3 dir = translation_ - otherPos;

        accelerationRefl_ = dir * reflectionPower_;
    }
}

void Enemy::OnCollisionTrigger(const Collider* _other)
{
    if (_other->GetColliderID() == "playerBullet")
    {
        isAlive_ = false;
    }

}

void Enemy::DebugWindow()
{
    BaseObject::DebugWindow();
    ImGui::Checkbox("Draw2D Collision Area", &isDrawCollisionArea_);
}

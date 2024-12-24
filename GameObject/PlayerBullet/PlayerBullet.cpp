#include "PlayerBullet.h"

void PlayerBullet::Initialize()
{
    name_ = "playerBullet";

    timer_ = std::make_unique<Timer>();
    timer_->Start();

    object_ = std::make_unique<Object3d>();
    object_->Initialize("Cube.obj");
    object_->SetScale(Vector3(0.3f, 0.3f, 0.3f));
    object_->SetTranslate(Vector3(0, 0.5f, 0));
    object_->SetRotate(Vector3(0, 0, 0));
}

void PlayerBullet::Finalize()
{
    object_->Finalize();
    object_.reset();
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
    object_->Update();
}

void PlayerBullet::Draw()
{
    object_->Draw();
}

void PlayerBullet::ModifyGameEye(GameEye* _eye)
{
    object_->SetGameEye(_eye);
}

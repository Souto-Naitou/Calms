#include "Player.h"

void Player::Initialize()
{
    object_ = std::make_unique<Object3d>();
    object_->Initialize("Cube.obj");
    object_->SetTranslate(Vector3(0, 0, 0));
    object_->SetRotate(Vector3(0, 0, 0));
}

void Player::Finalize()
{
    object_->Finalize();
    object_.reset();
}

void Player::Update()
{
    object_->Update();
}

void Player::Draw()
{
    object_->Draw();
}
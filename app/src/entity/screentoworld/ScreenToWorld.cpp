#include "ScreenToWorld.h"

#include <Core/Window/Window.h>
#include <config/ResourcePath.h>

void ScreenToWorld::Initialize()
{
    reticle_ = std::make_unique<Sprite>();
    reticle_->Initialize(Path::Image::InGame::kReticle);
    reticle_->SetSizeWithFactor(0.2f);
    reticle_->SetAnchorPoint({ 0.5f, 0.5f });
    normal_ = Vector3(0, 1, 0);
}

void ScreenToWorld::Finalize()
{
    reticle_->Finalize();
}

void ScreenToWorld::Update()
{
    // マウス座標を取得
    POINT cursor = pInput_->GetCursorPosition();
    Vector3 mousePosNear    = { static_cast<float>(cursor.x), static_cast<float>(cursor.y), 0.0f };
    Vector3 mousePosFar     = { static_cast<float>(cursor.x), static_cast<float>(cursor.y), 1.0f };

    Matrix4x4 vpMatrix = pGameEye_->GetViewProjectionMatrix();

    Matrix4x4 viewportMatrix = Matrix4x4::ViewportMatrix(
        0,
        0,
        static_cast<float>(Window::clientWidth),
        static_cast<float>(Window::clientHeight),
        0.0f,
        1.0f
    );

    Matrix4x4 vpvMatrix = vpMatrix * viewportMatrix;
    Matrix4x4 invVPVMatrix = vpvMatrix.Inverse();

    // マウス座標をワールド座標に変換
    Vector3 nearPos = FMath::Transform(mousePosNear, invVPVMatrix);
    Vector3 farPos = FMath::Transform(mousePosFar, invVPVMatrix);

    origin_ = pGameEye_->GetTransform().translate;
    direction_ = farPos - nearPos;
    direction_ = direction_.Normalized();
    
    float dot = FMath::Dot(normal_, direction_);
    if (dot == 0.0f)
    {
        return;
    }

    float t = (0 - FMath::Dot(normal_, origin_)) / dot;

    if (t < 0.0f)
    {
        return;
    }

    worldPoint_ = origin_ + direction_ * t;

    nearPos.y = 0;
    reticle_->SetPosition({ static_cast<float>(cursor.x), static_cast<float>(cursor.y) });
    reticle_->Update();
}

void ScreenToWorld::Draw1F()
{
    reticle_->Draw1F();
}
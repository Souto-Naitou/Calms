#include "ScreenToWorld.h"

#include <Core/Win32/WinSystem.h>

void ScreenToWorld::Initialize()
{
    object_ = new Object3d();
    object_->Initialize("Cube.obj");
    object_->SetScale(Vector3(0.3f, 0.3f, 0.3f));
    object_->SetTranslate(Vector3(0, 0, 0));
    object_->SetRotate(Vector3(0, 0, 0));

    normal_ = Vector3(0, 1, 0);
}

void ScreenToWorld::Finalize()
{
    object_->Finalize();
    delete object_;
}

void ScreenToWorld::Update()
{
    // マウス座標を取得
    POINT cursor = {};
    GetCursorPos(&cursor);
    ScreenToClient(WinSystem::GetInstance()->GetHwnd(), &cursor);
    Vector3 mousePosNear = { (float)cursor.x, (float)cursor.y, 0.0f };
    Vector3 mousePosFar = { (float)cursor.x, (float)cursor.y, 1.0f };

    Matrix4x4 vpMatrix = pGameEye_->GetViewProjectionMatrix();
    Matrix4x4 viewportMatrix = Matrix4x4::ViewportMatrix(0,0, WinSystem::kClientWidth, WinSystem::kClientHeight, 0.0f, 1.0f);
    Matrix4x4 vpvMatrix = vpMatrix * viewportMatrix;
    Matrix4x4 invVPVMatrix = vpvMatrix.Inverse();

    // マウス座標をワールド座標に変換
    Vector3 nearPos = FMath::Transform(mousePosNear, invVPVMatrix);
    Vector3 farPos = FMath::Transform(mousePosFar, invVPVMatrix);

    origin_ = pGameEye_->GetTransform().translate;
    direction_ = farPos - nearPos;
    direction_ = direction_.Normalize();
    
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
    object_->SetTranslate(worldPoint_);
    object_->Update();
}

void ScreenToWorld::Draw()
{
    object_->Draw();
}

void ScreenToWorld::ModifyGameEye(GameEye* _eye)
{
    object_->SetGameEye(_eye);
}

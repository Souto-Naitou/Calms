#include "BaseObject.h"

#include <DebugTools/DebugManager/DebugManager.h>
#include <MathExtension/mathExtension.h>
#include <imgui.h>

void BaseObject::Initialize()
{
    DebugManager::GetInstance()->SetComponent("GameObject", name_, std::bind(&BaseObject::DebugWindow, this));
}

void BaseObject::Finalize()
{
    DebugManager::GetInstance()->DeleteComponent("GameObject", name_.c_str());
}

void BaseObject::UpdateTransform(float _dt)
{
    // 加速度から速度を更新
    velocity_ += acceleration_ * _dt;

    /// 摩擦をかける
    Math::clamp(friction_, 0.0f, 1.0f);
    velocity_ *= friction_;

    // 速度から位置を更新
    translation_ += velocity_ * _dt;

    // 加速度をリセット
    acceleration_ = {};
}

void BaseObject::DebugWindow()
{
#ifdef DEBUG
    ImGui::DragFloat3("Scale", &scale_.x, 0.12f);
    ImGui::DragFloat3("Rotation", &rotation_.x, 0.12f);
    ImGui::DragFloat3("Translation", &translation_.x, 0.12f);
    ImGui::DragFloat3("Velocity", &velocity_.x, 0.12f);
    ImGui::DragFloat3("Acceleration", &acceleration_.x, 0.12f);
    ImGui::SliderFloat("Friction", &friction_, 0.0f, 1.0f);
#endif // DEBUG
}

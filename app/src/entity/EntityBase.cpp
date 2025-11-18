#include "EntityBase.h"

#include <DebugTools/DebugManager/DebugManager.h>
#include <MathExtension/mathExtension.h>
#include <imgui.h>

void EntityBase::Initialize(const EntityCommonParams& params, bool enableDebugWindow /*= true*/)
{
    isEnableDebugWindow_ = enableDebugWindow;
    if (isEnableDebugWindow_)
    {
        pDebugEntry_ = std::make_unique<DebugEntry<EntityBase>>("GameObject", this);
    }
    commonParams_ = params;
}

void EntityBase::UpdatePhysics(float dt)
{
    // 加速度から速度を更新
    velocity_ += acceleration_ * dt;

    /// 摩擦をかける
    Math::clamp(friction_, 0.0f, 1.0f);
    velocity_ *= friction_;

    // 速度から位置を更新
    transform_.translate += velocity_ * dt;

    // 加速度をリセット
    acceleration_ = {};
}

void EntityBase::ImGui()
{
#ifdef _DEBUG

    ImGui::DragFloat3("Scale", &transform_.scale.x, 0.12f);
    ImGui::DragFloat3("Rotation", &transform_.rotate.x, 0.12f);
    ImGui::DragFloat3("Translation", &transform_.translate.x, 0.12f);
    ImGui::DragFloat3("Velocity", &velocity_.x, 0.12f);
    ImGui::DragFloat3("Acceleration", &acceleration_.x, 0.12f);
    ImGui::SliderFloat("Friction", &friction_, 0.0f, 1.0f);

#endif // DEBUG
}

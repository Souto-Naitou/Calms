#include "EntityBase.h"

#include <DebugTools/DebugManager/DebugManager.h>
#include <MathExtension/mathExtension.h>
#include <imgui.h>
#include <drawable/object3d/Object3dSystem.h>

void EntityBase::Initialize(bool enableDebugWindow /*= true*/)
{
    if (enableDebugWindow)
    {
        pDebugEntry_ = std::make_unique<DebugEntry<EntityBase>>("GameObject", this);
    }

    ppGameEye_ = Object3dSystem::GetInstance()->GetGlobalEye();
}

void EntityBase::ImGui() 
{
    #ifdef _DEBUG

    ImGui::Text("Alive : %s", isAlive_ ? "True" : "False");

    #endif // _DEBUG
}

void EntityBase::SetName(const std::string& name)
{
    if (pDebugEntry_) pDebugEntry_->SetName(name);
}

void EntityBase::ShakeCamera(float power)
{
    (*ppGameEye_)->Shake(power);
}

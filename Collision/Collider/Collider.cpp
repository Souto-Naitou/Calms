#include "Collider.h"
#include <Collision/Manager/CollisionManager.h>
#include <DebugTools/ImGuiTemplates/ImGuiTemplates.h>
#include <sstream>

Collider::Collider()
{
}

Collider::~Collider()
{
    DebugManager::GetInstance()->DeleteComponent("Colliders", colliderID_.c_str());
}

void Collider::DrawArea()
{
    switch (shape_)
    {
    case Shape::AABB:
        std::get<AABB*>(shapeData_)->Draw();
        break;
    case Shape::OBB:
        std::get<OBB*>(shapeData_)->Draw();
        break;
    case Shape::Sphere:
        break;
    default:
        break;
    }
}

const bool Collider::IsRegisteredCollidingPtr(const Collider* _ptr) const
{
    for (auto itr = collidingPtrList_.begin(); itr != collidingPtrList_.end(); ++itr)
    {
        if (_ptr == *itr) return true;
    }
    return false;
}

void Collider::EraseCollidingPtr(const Collider* _ptr)
{
    collidingPtrList_.remove_if([_ptr](const Collider* _pCollider) {
        return _pCollider == _ptr;
    });
    return;
}

void Collider::SetAttribute(uint32_t _attribute)
{
    collisionAttribute_ = _attribute;
}

void Collider::SetMask(uint32_t* _mask)
{
    pCollisionMask_ = _mask;
}

void Collider::OnCollisionTrigger(const Collider* _other)
{
    if (onCollisionTriggerFunction_)
        onCollisionTriggerFunction_(_other);
    return;
}

void Collider::DebugWindow()
{
#ifdef _DEBUG
    ImGui::Text("Attribute: %x", collisionAttribute_);
    auto pFunc = [&]()
    {
        for (auto ptr : collidingPtrList_)
        {
            std::stringstream ss;
            ss << "0x" << std::hex << reinterpret_cast<uintptr_t>(ptr);
            ImGuiTemplate::VariableTableRow(ss.str(), ptr->GetColliderID());
        }
    };

    ImGuiTemplate::VariableTable("Collider", pFunc);
#endif // _DEBUG
}

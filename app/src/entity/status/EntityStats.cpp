#include "EntityStats.h"

#include <DebugTools/Logger/Logger.h>
#include <imgui.h>
#include <typeinfo>
#include <cassert>

void EntityStats::Initialize(float health, float attack, float speed)
{
    health_ = health;
    maxHealth_ = health;
    attack_ = attack;
    speed_ = speed;
    score_ = 0;
}

void EntityStats::Update()
{
    // 整合性チェック
    if (health_ > maxHealth_)
    {
        health_ = maxHealth_;
    }
}

void EntityStats::ImGui(const std::string& name) const
{
    #ifdef _DEBUG
    ImGui::Text("HP: %.1f / %.1f", health_, maxHealth_);
    ImGui::Text("Attack: %.1f", attack_);
    ImGui::Text("Speed: %.1f", speed_);
    #endif
}

void EntityStats::OnCollision(const IEntityStats* status)
{
    ///=================
    ///==== Calc =======
    ///=================
    assert(status != nullptr);
    EntityStats* casted = nullptr;

    try
    {
        casted = static_cast<EntityStats*>(const_cast<IEntityStats*>(status));
    }
    catch (const std::bad_cast& e)
    {
        Logger::GetInstance()->LogError(__FILE__, __FUNCTION__, e.what());
        assert(false);
        return;
    }

    // ダメージ
    float damage = casted->attack_;
    if (damage < 0.0f)
    {
        damage = 0.0f;
    }

    ///=================
    ///==== Apply ======
    ///=================

    // ダメージ
    health_ -= damage;
    if (health_ < 0.0f)
    {
        health_ = 0.0f;
    }
}

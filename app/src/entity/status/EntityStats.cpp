#include "EntityStats.h"

#include <DebugTools/Logger/Logger.h>
#include <imgui.h>
#include <typeinfo>
#include <cassert>
#include <algorithm>
#undef max

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

void EntityStats::OnCollision(const IEntityStats* other)
{
    /// [ 型チェック ]
    assert(other != nullptr);

    /// [ ダメージ計算 ]
    float damage = other->GetDamage();
    damage = std::max(damage, 0.0f);

    /// [ 適用 ]

    // ダメージ
    health_ -= damage;
    if (health_ < 0.0f)
    {
        health_ = 0.0f;
    }
}
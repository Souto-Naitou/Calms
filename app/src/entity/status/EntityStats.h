#pragma once

#include <string>
#include <Interfaces/IEntityStats.h>

/// <summary>
/// エンティティのステータスクラス
/// </summary>
class EntityStats : public IEntityStats
{
public:
    EntityStats() = default;
    ~EntityStats() = default;

    void Initalize(float health, float attack, float speed);
    void Update();
    void ImGui(const std::string& name) const;

    // Getter
    float GetHp() const { return health_; }
    float GetMaxHp() const { return maxHealth_; }
    float GetAttack() const { return attack_; }
    float GetSpeed() const { return speed_; }

    // Setter
    EntityStats& SetHp(float hp) { health_ = hp; return *this; }
    EntityStats& SetMaxHp(float maxHp) { maxHealth_ = maxHp; return *this; }
    EntityStats& SetAttack(float attack) { attack_ = attack; return *this; }
    EntityStats& SetSpeed(float speed) { speed_ = speed; return *this; }

    // Adder
    void AddHp(float hp) { health_ += hp; }
    void AddAttack(float attack) { attack_ += attack; }
    void AddSpeed(float speed) { speed_ += speed; }

    void OnCollision(const IEntityStats* status) override;

private:
    float               health_ = 0;
    float               maxHealth_ = 0;
    float               attack_ = 0;
    float               speed_ = 0;
    float               score_ = 0;
};
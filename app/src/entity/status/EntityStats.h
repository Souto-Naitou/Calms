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

    /// <summary>
    /// ステータス値を初期化します。
    /// </summary>
    /// <param name="health">初期HP。</param>
    /// <param name="attack">初期攻撃力。</param>
    /// <param name="speed">初期移動速度。</param>
    void Initialize(float health, float attack, float speed);

    /// <summary>
    /// 継続的に変化する値やクールダウン等の更新を行います。
    /// </summary>
    void Update();

    /// <summary>
    /// ステータスのデバッグ表示を行います。
    /// </summary>
    /// <param name="name">表示名。</param>
    void ImGui(const std::string& name) const;

    // Getter
    float GetHp() const { return health_; }
    float GetMaxHp() const { return maxHealth_; }
    float GetSpeed() const { return speed_; }
    float GetDamage() const override { return attack_; }

    // Setter
    EntityStats& SetHp(float hp) { health_ = hp; return *this; }
    EntityStats& SetMaxHp(float maxHp) { maxHealth_ = maxHp; return *this; }
    EntityStats& SetAttack(float attack) { attack_ = attack; return *this; }
    EntityStats& SetSpeed(float speed) { speed_ = speed; return *this; }

    // Adder
    void AddHp(float hp) { health_ += hp; }
    void AddAttack(float attack) { attack_ += attack; }
    void AddSpeed(float speed) { speed_ += speed; }

    /// <summary>
    /// 衝突時の処理を行います（ダメージ計算など）。
    /// </summary>
    /// <param name="status">相手のステータス。</param>
    void OnCollision(const IEntityStats* status) override;

private:
    float               health_ = 0;
    float               maxHealth_ = 0;
    float               attack_ = 0;
    float               speed_ = 0;
    float               score_ = 0;
};
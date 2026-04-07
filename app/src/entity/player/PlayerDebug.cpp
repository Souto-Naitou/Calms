#include "PlayerDebug.h"


void PlayerDebug::ImGui(PlayerExplosionTrigger& explosionTrigger)
{
    #ifdef _DEBUG

    if (ImGui::CollapsingHeader("ExplosionTrigger"))
    {
        float decreaseTimeFraction = explosionTrigger.GetDecreaseProgress();
        if (decreaseTimeFraction > 1.0f) decreaseTimeFraction = 1.0f;
        ImGui::ProgressBar(decreaseTimeFraction);
        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("Until Decrease");
    }

    #endif // _DEBUG
}

void PlayerDebug::ImGui(EntityStats& stats)
{
    #ifdef _DEBUG

    constexpr float kInvincibilityHp = 999999.0f;

    // 初期化
    if (!maxHpDefault_.has_value())
    {
        maxHpDefault_ = stats.GetMaxHp();
    }


    if (ImGui::CollapsingHeader("PlayerStats"))
    {
        if (ImGui::Checkbox("Invincibility", &enableInvincibility_))
        {
            if (enableInvincibility_)
            {
                stats.SetMaxHp(kInvincibilityHp);
                stats.SetHp(kInvincibilityHp);
            }
            else // 無敵化解除
            {
                stats.SetMaxHp(maxHpDefault_.value());
                if (stats.GetHp() > maxHpDefault_.value())
                {
                    stats.SetHp(maxHpDefault_.value());
                }
            }
        }

    }

    #endif // _DEBUG
}

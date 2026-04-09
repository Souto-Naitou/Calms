#pragma once
#include "PlayerExplosionTrigger.h"
#include <entity/status/EntityStats.h>

class PlayerDebug
{
public:
    void ImGui(PlayerExplosionTrigger& explosionTrigger);
    void ImGui(EntityStats& stats);

private:
    std::optional<float>    maxHpDefault_           = std::nullopt; // !< デフォルトの最大HP（無敵化解除時に使用）
    bool                    enableInvincibility_    = false;        // !< 無敵化の有効フラグ
};
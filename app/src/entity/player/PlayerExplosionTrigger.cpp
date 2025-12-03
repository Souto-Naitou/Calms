#include "PlayerExplosionTrigger.h"
#include <functional>


void PlayerExplosionTrigger::Initialize()
{
    subscription_ = EventListener::GetInstance()->Subscribe<KillEnemyEvent>(
        std::bind(
            &PlayerExplosionTrigger::OnKillEnemyEvent,
            this,
            std::placeholders::_1
        )
    );
}

void PlayerExplosionTrigger::OnKillEnemyEvent(const KillEnemyEvent& payload)
{
    currentScore_ += kScorePerEnemy;
}

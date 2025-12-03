#pragma once
#include <Features/Event/EventListener.h>
#include <logic/event/KillEnemyEvent.h>
#include <optional>

class PlayerExplosionTrigger
{
public:
    PlayerExplosionTrigger() = default;
    ~PlayerExplosionTrigger() = default;

    void Initialize();
    void OnKillEnemyEvent(const KillEnemyEvent& payload);

private:
    static constexpr float kScorePerEnemy = 10.0f;
    std::optional<EventSubscription> subscription_;
    const float targetTriggerScore_ = 100.0f;
    float currentScore_ = 0.0f;
};
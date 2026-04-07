#pragma once
#include "PlayerInput.h"
#include <logic/event/KillEnemyEvent.h>
#include <Features/TimeMeasurer/TimeMeasurerByDt.h>
#include <Features/Event/EventSubscription.h>
#include <optional>
#include <memory>
#include "PlayerContext.h"

class PlayerExplosionTrigger
{
public:
    PlayerExplosionTrigger(PlayerInput& input, PlayerContext& context) : input_(input), context_(context) {}
    ~PlayerExplosionTrigger() = default;

    void Initialize();
    void Update();
    void OnKillEnemyEvent(const KillEnemyEvent&);

    float GetDecreaseProgress();

private:
    void DecreaseScore();
    void UpdateTriggerIf();

    static constexpr float kScorePerEnemy = 20.0f;
    static constexpr float kDecreaseBeginTime = 3.0f;
    static constexpr float kDecreasePerSec = 20.0f; // 20 points per second
    static constexpr float kTargetTriggerScore_ = PlayerContext::kMaxExplosionScore;

    PlayerInput&                        input_;
    PlayerContext&                      context_;
    TimeMeasurerByDt                    decreaseTimer_ = {};
    std::optional<EventSubscription>    subscription_   = std::nullopt;
};
#pragma once
#include <logic/event/KillEnemyEvent.h>
#include <Features/TimeMeasurer/TimeMeasurerByDt.h>
#include <Features/Event/EventSubscription.h>
#include <optional>
#include <memory>

class PlayerExplosionTrigger
{
public:
    PlayerExplosionTrigger() = default;
    ~PlayerExplosionTrigger() = default;

    void Initialize();
    void Update();
    void ImGui();
    void OnKillEnemyEvent(const KillEnemyEvent& payload);

private:
    void DecreaseScore();

    static constexpr float kScorePerEnemy = 5.0f;
    static constexpr float kDecreaseBeginTime = 1.0f;
    static constexpr float kDecreasePerSec = 20.0f; // 20 points per second

    const float targetTriggerScore_ = 100.0f;

    std::unique_ptr<TimeMeasurerByDt>   decreaseTimer_ = {};
    std::optional<EventSubscription>    subscription_ = std::nullopt;
    float                               currentScore_ = 0.0f;
};
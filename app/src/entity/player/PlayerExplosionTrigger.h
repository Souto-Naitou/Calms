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
    PlayerExplosionTrigger() = default;
    ~PlayerExplosionTrigger() = default;

    void Initialize(PlayerInput* pInput, PlayerContext* pContext);
    void Update();
    void ImGui();
    void OnKillEnemyEvent(const KillEnemyEvent&);

private:
    void DecreaseScore();
    void UpdateTriggerIf();

    static constexpr float kScorePerEnemy = 20.0f;
    static constexpr float kDecreaseBeginTime = 1.0f;
    static constexpr float kDecreasePerSec = 20.0f; // 20 points per second
    static constexpr float kTargetTriggerScore_ = 100.0f;

    PlayerInput*                        pInput_         = nullptr;
    PlayerContext*                      pContext_         = nullptr;
    std::unique_ptr<TimeMeasurerByDt>   decreaseTimer_  = {};
    std::optional<EventSubscription>    subscription_   = std::nullopt;
};
#include "PlayerExplosionTrigger.h"

#include <Features/Event/EventListener.h>
#include <functional>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <imgui.h>
#include <logic/event/PlayerExplosionEvent.h>

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

void PlayerExplosionTrigger::Update()
{
    decreaseTimer_.Update(static_cast<uint32_t>(DeltaTimeChannelReserved::Game));

    if (decreaseTimer_.GetNow<float>() > kDecreaseBeginTime)
    {
        this->DecreaseScore();
    }

    this->UpdateTriggerIf();
}

void PlayerExplosionTrigger::OnKillEnemyEvent(const KillEnemyEvent&)
{
    context_.IncreaseExplosionScore(kScorePerEnemy);
    decreaseTimer_.Reset();
    decreaseTimer_.Start();
}

float PlayerExplosionTrigger::GetDecreaseProgress()
{
    const float now = decreaseTimer_.GetNow<float>();
    float decreaseProgress = now / kDecreaseBeginTime;
    if (decreaseProgress > 1.0f) decreaseProgress = 1.0f;
    return decreaseProgress;
}

void PlayerExplosionTrigger::DecreaseScore()
{
    auto channel = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    float dt = DeltaTimeManager::GetInstance()->GetDeltaTime(channel);
    context_.DecreaseExplosionScore(kDecreasePerSec * dt);
}

void PlayerExplosionTrigger::UpdateTriggerIf()
{    
    bool doExplosion = input_.GetData().isExplosionTriggered;
    doExplosion &= (context_.Get().explosionScore >= kTargetTriggerScore_);

    if (doExplosion)
    {
        /// トリガーイベント発行
        context_.ResetExplosionScore();
        decreaseTimer_.Reset();
        EventListener::GetInstance()->Publish(PlayerExplosionEvent{});
    }
}

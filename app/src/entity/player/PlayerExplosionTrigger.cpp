#include "PlayerExplosionTrigger.h"

#include <Features/Event/EventListener.h>
#include <functional>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <imgui.h>
#include <logic/event/PlayerExplosionEvent.h>

void PlayerExplosionTrigger::Initialize(PlayerInput* pInput, PlayerContext* pContext)
{
    pInput_ = pInput;
    pContext_ = pContext;
    subscription_ = EventListener::GetInstance()->Subscribe<KillEnemyEvent>(
        std::bind(
            &PlayerExplosionTrigger::OnKillEnemyEvent,
            this,
            std::placeholders::_1
        )
    );

    decreaseTimer_ = std::make_unique<TimeMeasurerByDt>();
}

void PlayerExplosionTrigger::Update()
{
    decreaseTimer_->Update(static_cast<uint32_t>(DeltaTimeChannelReserved::Game));

    if (decreaseTimer_->GetNow<float>() > kDecreaseBeginTime)
    {
        this->DecreaseScore();
    }

    this->UpdateTriggerIf();
}

void PlayerExplosionTrigger::ImGui()
{
    #ifdef _DEBUG
    float decreaseTimeFraction = decreaseTimer_->GetNow<float>() / kDecreaseBeginTime;
    if (decreaseTimeFraction > 1.0f) decreaseTimeFraction = 1.0f;
    ImGui::ProgressBar(decreaseTimeFraction);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("Until Decrease");
    #endif // _DEBUG
}

void PlayerExplosionTrigger::OnKillEnemyEvent(const KillEnemyEvent&)
{
    pContext_->IncreaseExplosionScore(kScorePerEnemy);
    decreaseTimer_->Reset();
    decreaseTimer_->Start();
}

void PlayerExplosionTrigger::DecreaseScore()
{
    auto channel = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    float dt = DeltaTimeManager::GetInstance()->GetDeltaTime(channel);
    pContext_->DecreaseExplosionScore(kDecreasePerSec * dt);
}

void PlayerExplosionTrigger::UpdateTriggerIf()
{
    if (!pInput_) return;
    
    bool doExplosion = pInput_->GetData().isExplosionTriggered;
    doExplosion &= (pContext_->Get().explosionScore >= kTargetTriggerScore_);

    if (doExplosion)
    {
        // トリガー発動
        pContext_->ResetExplosionScore();
        decreaseTimer_->Reset();
        EventListener::GetInstance()->Publish(PlayerExplosionEvent{});
    }
}

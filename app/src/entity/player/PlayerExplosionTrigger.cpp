#include "PlayerExplosionTrigger.h"

#include <Features/Event/EventListener.h>
#include <functional>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <imgui.h>
#include <logic/event/PlayerExplosionEvent.h>

void PlayerExplosionTrigger::Initialize(PlayerInput* pInput)
{
    pInput_ = pInput;
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

    ImGui::ProgressBar(currentScore_ / targetTriggerScore_);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("Current Score");

    float decreaseTimeFraction = decreaseTimer_->GetNow<float>() / kDecreaseBeginTime;
    if (decreaseTimeFraction > 1.0f) decreaseTimeFraction = 1.0f;
    ImGui::ProgressBar(decreaseTimeFraction);
    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("Until Decrease");

    #endif // _DEBUG
}

void PlayerExplosionTrigger::OnKillEnemyEvent([[maybe_unused]]const KillEnemyEvent& payload)
{
    currentScore_ += kScorePerEnemy;
    if (currentScore_ > targetTriggerScore_) currentScore_ = targetTriggerScore_;
    decreaseTimer_->Reset();
    decreaseTimer_->Start();
}

void PlayerExplosionTrigger::DecreaseScore()
{
    auto channel = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    float dt = DeltaTimeManager::GetInstance()->GetDeltaTime(channel);
    currentScore_ -= kDecreasePerSec * dt;
    if (currentScore_ < 0.0f) currentScore_ = 0.0f;
}

void PlayerExplosionTrigger::UpdateTriggerIf()
{
    if (!pInput_) return;
    
    bool doExplosion = pInput_->GetData().isExplosionTriggered;
    doExplosion &= (currentScore_ >= targetTriggerScore_);

    if (doExplosion)
    {
        // トリガー発動
        currentScore_ = 0.0f;
        decreaseTimer_->Reset();
        EventListener::GetInstance()->Publish(PlayerExplosionEvent{});
    }
}

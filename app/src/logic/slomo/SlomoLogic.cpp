#include "SlomoLogic.h"
#include <algorithm>

SlomoLogic::State SlomoLogic::Update(bool isSlomoActive, DeltaTimeManager* deltaTimeManager)
{
    State state{};
    state.remainingTime = remainingTime_;
    state.cooldownTime = kSlomoTimeMax_ - remainingTime_;

    bool isSlomoJustActivated = isSlomoActive && (remainingTime_ > 0.0f);
    isSlomoJustActivated &= isSlomoActivePrev_ || remainingTime_ > kSlomoTimeUsableMin_;

    if (isSlomoJustActivated)
    {
        /// [ ゲームの進行速度を遅くする ]
        deltaTimeManager->SetDeltaTime(DeltaTimeChannelReserved::Game, kDeltaTimeSlomoGame_);
        deltaTimeManager->SetDeltaTime(DeltaTimeChannelReserved::Particle, kDeltaTimeSlomoParticle_);
        /// [ スロモ時間を減少させる ]
        remainingTime_ -= kDeltaTimeDefault_;
    }
    else
    {
        /// [ 通常速度に戻す ]
        deltaTimeManager->SetDeltaTime(DeltaTimeChannelReserved::Game, kDeltaTimeDefault_);
        deltaTimeManager->SetDeltaTime(DeltaTimeChannelReserved::Particle, kDeltaTimeDefault_);
        /// [ スロモ時間を回復させる ]
        remainingTime_ += kSlomoTimeIncrementPerFrame_;
    }

    // スロモ時間の制限
    remainingTime_ = std::clamp(remainingTime_, 0.0f, kSlomoTimeMax_);

    state.isSlomoActive = isSlomoJustActivated;
    isSlomoActivePrev_ = state.isSlomoActive;

    return state;
}

#pragma once
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>

/// <summary>
/// スロモロジッククラス
/// スロモの処理の内、ゲームの進行に関係する部分を担当します。
/// </summary>
class SlomoLogic
{
public:
    constexpr static float  kSlomoTimeMax_ = 5.0f;

    SlomoLogic() = default;

    struct State
    {
        bool isSlomoActive = false;
        float remainingTime = 0.0f;
        float cooldownTime = 0.0f;
    };

    /// <summary>
    /// スロモ状態の更新を行います。
    /// </summary>
    /// <param name="isSlomoActive">スロモがアクティブかどうか。</param>
    /// <param name="deltaTimeManager">デルタタイムマネージャー。</param>
    State Update(bool isSlomoActive, DeltaTimeManager* deltaTimeManager);

    inline float GetRemainingTime() const { return remainingTime_; }

private:
    constexpr static float  kDeltaTimeDefault_              = 1.0f / 60.0f;
    constexpr static float  kDeltaTimeSlomoGame_            = 1.0f / 120.0f;
    constexpr static float  kDeltaTimeSlomoParticle_        = 1.0f / 180.0f;
    constexpr static float  kSlomoTimeIncrementPerFrame_    = 1.0f / 60.0f;
    constexpr static float  kSlomoTimeUsableMin_            = 1.0f;
    float                   remainingTime_                  = kSlomoTimeMax_;
    bool                    isSlomoActivePrev_              = false;
};
#include "WaveBadgetCalculator.h"
#include <cmath>
#include <cassert>



void WaveBadgetCalculator::Initialize(const Config& cfg)
{
    config_ = cfg;
}

void WaveBadgetCalculator::Apply(WaveInfo& info) const
{
    const float kWaveIndex = static_cast<float>(info.waveIndex);
    const float kMaxBudget = static_cast<float>(config_.maxBudget);
    const float kMaxBudgetWaveIndex = static_cast<float>(config_.maxBudgetWaveIndex);
    
    // 最大予算に達するウェーブインデックスが0の場合、すべてのウェーブで最大予算を使用 (ゼロ除算を防止)
    if (kMaxBudgetWaveIndex == 0)
    {
        assert(false && "max budget wave index must be greater than 0");
        info.budget = static_cast<int>(kMaxBudget);
        return;
    }

    // 正規化された現在のウェーブインデックス（0.0から1.0の範囲）
    const float kNowIndexNormalized = static_cast<float>(kWaveIndex) / static_cast<float>(kMaxBudgetWaveIndex);

    // 線形補間を使用して予算を計算
    float budget = std::lerp(0.0f, kMaxBudget, kNowIndexNormalized);

    info.budget = static_cast<int>(budget);
}

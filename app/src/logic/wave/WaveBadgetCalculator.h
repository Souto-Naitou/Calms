#pragma once
#include "WaveInfo.h"

/// ウェーブごとの予算を計算するクラス
/// 使用先例 : WaveInfoBuilder
class WaveBadgetCalculator
{
public:

    struct Config
    {
        int maxBudget = 1000; // !< ウェーブが所有できる最大予算
        int maxBudgetWaveIndex = 50; // !< 最大予算に達するウェーブのインデックス
    };

    void Initialize(const Config& cfg);
    void Apply(WaveInfo& info) const;

private:
    Config config_;
};
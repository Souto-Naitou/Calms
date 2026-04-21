#pragma once
#include "WaveInfo.h"
#include "WaveBadgetCalculator.h"

/// WaveInfoを組み立てるクラス
class WaveInfoBuilder
{
public:
    void Initialize();
    WaveInfo Build();

private:
    WaveBadgetCalculator badgetCalculator_; // !< ウェーブの予算を計算するためのクラス
};
#include "WaveInfoBuilder.h"



void WaveInfoBuilder::Initialize()
{
    WaveBadgetCalculator::Config badgetCfg;
    badgetCfg.maxBudget = 1000;
    badgetCfg.maxBudgetWaveIndex = 25;
    badgetCalculator_.Initialize(badgetCfg);
}

WaveInfo WaveInfoBuilder::Build()
{
    WaveInfo info;
    badgetCalculator_.Apply(info);
    return info;
}

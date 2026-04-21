#include "EnemyTypeSelector.h"
#include <features/RandomGenerator/RandomGenerator.h>


EnemyType EnemyTypeSelector::GetRandom(EnemyTypeList& enables)
{
    const size_t enablesCount = enables.size();
    const float indexRandomized = RandomGenerator::GetInstance()->Generate<float>(0, static_cast<float>(enablesCount));
    return enables.at(static_cast<size_t>(indexRandomized));
}

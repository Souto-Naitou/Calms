#pragma once
#include <entity/enemy/EnemyType.h>

struct KillEnemyEvent
{
    EnemyType enemyType = EnemyType::Normal;
    float scoreMultiplier = 1.0f;
};
#pragma once
#include <entity/enemy/EnemyTypes.h>

struct KillEnemyEvent
{
    EnemyTypes enemyType = EnemyTypes::Normal;
    float scoreMultiplier = 1.0f;
};
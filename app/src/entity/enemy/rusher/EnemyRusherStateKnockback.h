#pragma once

#include <entity/enemy/Rusher/EnemyRusherState.h>

class EnemyRusherStateKnockback : public EnemyRusherState
{
public:
    void Enter(EnemyRusher& enemy) override;
    void Update(EnemyRusher& enemy, float deltaTime) override;
};
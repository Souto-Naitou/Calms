#pragma once

#include <entity/enemy/Rusher/EnemyRusherState.h>
#include <entity/enemy/Rusher/EnemyRusher.h>

class EnemyRusherStateDash : public EnemyRusherState
{
public:
    void Enter(EnemyRusher& enemy) override;
    void Update(EnemyRusher& enemy, float deltaTime) override;
};
#pragma once

#include <entity/enemy/Rusher/EnemyRusherState.h>
#include <entity/enemy/Rusher/EnemyRusher.h>

class EnemyRusherStateFollow : public EnemyRusherState
{
public:
    void Enter(EnemyRusher& enemy) override;
    void Update(EnemyRusher& enemy, float deltaTime) override;

private:
    static constexpr float kDistanceToStartRushing = 10.0f;
};
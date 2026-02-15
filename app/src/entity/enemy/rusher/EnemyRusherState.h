#pragma once

class EnemyRusher;

class EnemyRusherState
{
public:
    virtual void Enter(EnemyRusher& enemy) {};
    virtual void Update(EnemyRusher& enemy, float deltaTime) = 0;
    virtual void Exit(EnemyRusher& enemy) {};
};
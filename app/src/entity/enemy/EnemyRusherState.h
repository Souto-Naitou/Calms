#pragma once
#include "EnemyRusher.h"

class EnemyRusherState
{
public:
    virtual void Enter(EnemyRusher& enemy) {};
    virtual void Update(EnemyRusher& enemy) = 0;
    virtual void Exit(EnemyRusher& enemy) {};
};
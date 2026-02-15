#pragma once

#include <entity/enemy/Rusher/EnemyRusherState.h>
#include <entity/enemy/Rusher/EnemyRusher.h>

class EnemyRusherStateAim : public EnemyRusherState
{
public:
    void Enter(EnemyRusher& enemy) override;
    void Update(EnemyRusher& enemy, float deltaTime) override;

private:
    static constexpr float kAimDurationSec_ = 2.0f;
    static constexpr float kDashColorDurationSec_ = 0.3f;
    float elapsedTimeSec_ = 0.0f;
};
#include "EnemyRusherStateAim.h"
#include "EnemyRusherStateDash.h"


void EnemyRusherStateAim::Enter(EnemyRusher& enemy)
{
    enemy.DisableMovement();
}

void EnemyRusherStateAim::Update(EnemyRusher& enemy, float deltaTime)
{
    if (elapsedTimeSec_ >= kAimDurationSec_ - kDashColorDurationSec_)
    {
        enemy.ChangeColor(EnemyRusher::kColorDashing_.to_Vector4());
    }

    if (elapsedTimeSec_ < kAimDurationSec_)
    {
        enemy.FocusOnTarget(deltaTime);
    }
    else
    {
        // エイム時間が経過したら追尾状態に遷移
        enemy.ChangeState(std::make_unique<EnemyRusherStateDash>());
    }

    elapsedTimeSec_ += deltaTime;
}

#include "EnemyRusherStateKnockback.h"
#include "EnemyRusher.h"
#include "EnemyRusherStateFollow.h"



void EnemyRusherStateKnockback::Enter(EnemyRusher& enemy)
{
    enemy.ToPhysicsMovement();
}

void EnemyRusherStateKnockback::Update(EnemyRusher& enemy, float deltaTime)
{
    /// ターゲットに向かって注視
    enemy.FocusOnTarget(deltaTime);
    
    /// ノックバックが終了したら追尾状態に遷移
    if (enemy.IsStopped())
    {
        enemy.ChangeState(std::make_unique<EnemyRusherStateFollow>());
    }
}

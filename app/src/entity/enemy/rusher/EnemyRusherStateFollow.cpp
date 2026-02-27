#include "EnemyRusherStateFollow.h"
#include "EnemyRusherStateAim.h"
#include <memory>

void EnemyRusherStateFollow::Enter(EnemyRusher& enemy)
{
    /// 追尾移動に切り替え
    enemy.ToFollowMovement();
    enemy.ChangeColor(RGBA(0x17485cff).to_Vector4());
}

void EnemyRusherStateFollow::Update(EnemyRusher& enemy, float deltaTime)
{
    /// ターゲットに向かって注視・移動
    enemy.FocusOnTarget(deltaTime);

    /// ターゲットに近づいたらエイム状態に遷移
    if (enemy.IsCloseToTarget(kDistanceToStartRushing))
    {
        enemy.ChangeState(std::make_unique<EnemyRusherStateAim>());
    }
}
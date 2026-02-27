#include "EnemyRusherStateDash.h"
#include "EnemyRusherStateFollow.h"



void EnemyRusherStateDash::Enter(EnemyRusher& enemy)
{
    // ダッシュ移動に切り替え
    enemy.ToDashMovement();
    // ターゲットに向かってダッシュ開始
    enemy.DashToTarget();
}

void EnemyRusherStateDash::Update(EnemyRusher& enemy, [[maybe_unused]]float deltaTime)
{
    float elapsedTime = enemy.GetDashElapsedTime();
    float t = elapsedTime / EnemyRusher::kDashDurationSec_;
    Vector4 currentColor = {};
    currentColor.Lerp(
        EnemyRusher::kColorDashing_.to_Vector4(), 
        EnemyRusher::kColorDefault_.to_Vector4(), 
        t
    );
    enemy.ChangeColor(currentColor);

    if (!enemy.IsDashing())
    {
        // ダッシュが終了したら追尾状態に遷移
        enemy.ChangeState(std::make_unique<EnemyRusherStateFollow>());
    }
}

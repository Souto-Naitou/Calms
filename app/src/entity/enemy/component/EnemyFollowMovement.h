#pragma once

#include <entity/EntityMovement.h>

class EnemyFollowMovement : public EntityMovement
{
public:
    /// <summary>
    /// 敵の追跡移動を初期化します。
    /// </summary>
    /// <param name="pTargetPosition">追跡するターゲット位置へのポインタ。</param>
    EnemyFollowMovement(const Vector3* pTargetPosition);

    /// <summary>
    /// オイラー変換を時間差分に基づいて更新します。
    /// </summary>
    /// <param name="transform">更新するオイラー変換オブジェクトへの参照。</param>
    /// <param name="deltaTime">前回の更新からの経過時間(秒)。</param>
    void Update(EulerTransform& transform, float deltaTime) override;

    /// <summary>
    /// 追跡速度を設定します。
    /// </summary>
    /// <param name="speed">追跡速度(スカラー)</param>
    void SetFollowSpeed(float speed) { followSpeed_ = speed; }

private:
    float           followSpeed_        = 5.0f;
    const Vector3*  pTargetPosition_    = nullptr;
};
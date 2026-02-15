#pragma once
#include <component/IMovement.h>
#include <Math/Transform.h>
#include <Vector3.h>

class PhysicsMovement : public IMovement
{
public:
    /// <summary>
    /// 更新
    /// </summary>
    virtual void Update(EulerTransform& transform, float deltaTime) override;

    /// <summary>
    /// 指定された力を加速度に適用します。
    /// </summary>
    /// <param name="force">適用する力のベクトル。</param>
    void ApplyForce(const Vector3& force) { acceleration_ += force; }

    /// <summary>
    /// 速度をゼロにリセットします。
    /// </summary>
    void ResetVelocity() { velocity_ = Vector3(); }

    /// <summary>
    /// 速度に衝撃を加えます。
    /// 瞬間的に速度を変化させたい場合に使用します。
    /// </summary>
    /// <param name="impulse">加える衝撃ベクトル。</param>
    void AddImpulse(const Vector3& impulse);

    /// <summary>
    /// 移動を停止します。演出中などに使用します。
    /// </summary>
    void StopMovement();

    /// <summary>
    /// 摩擦を適用して速度を調整します。
    /// </summary>
    /// <param name="friction">速度に乗算する摩擦係数。</param>
    void ApplyFriction(float friction) { velocity_ *= friction; }

    /// [ Getters ]
    Vector3 GetVelocity() const { return velocity_; }
    Vector3 GetAcceleration() const { return acceleration_; }

protected:
    /// <summary>
    /// フレーム時間に基づいて位置を更新します。
    /// 毎フレーム呼び出すことを想定しています。
    /// </summary>
    /// <param name="position">更新される位置ベクトル。</param>
    /// <param name="deltaTime">前回の更新からの経過時間。</param>
    void PositionUpdate(Vector3& position, float deltaTime);

private:
    Vector3 acceleration_ = {};
    Vector3 velocity_ = {};
};
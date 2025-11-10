#pragma once

#include <Entity/EntityBase.h>
#include <drawable/object3d/Object3d.h>
#include <Vector3.h>
#include <memory>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Primitive/OBB.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>


/// <summary>
/// プレイヤー弾クラス
/// </summary>
class PlayerBullet : public EntityBase
{
public:
    PlayerBullet(IModel* _pModelSelfBody)
        : pModelSelfBody_(_pModelSelfBody) {};

    /// <summary>
    /// 弾の初期化を行います。
    /// モデル・コライダー等の準備を行います。
    /// </summary>
    /// <param name="_enableDebugWindow">デバッグウィンドウの有効/無効。</param>
    void Initialize(const EntityCommonParams& params, bool enableDebugWindow = true) override;

    /// <summary>
    /// 終了処理を行います。
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 弾の更新処理を行います。
    /// 移動、寿命管理、当たり判定などを更新します。
    /// </summary>
    void Update() override;

    /// <summary>
    /// 弾の描画処理を行います。
    /// </summary>
    void Draw1F() override;

    /// <summary>
    /// ライン等の補助描画を行います。
    /// </summary>
    void DrawLine() override;

    /// <summary>
    /// ImGui を用いたデバッグUIの描画を行います。
    /// </summary>
    void ImGui() override;


public: /// Setter
    void SetMoveVelocity(const Vector3& _velocity) { moveVelocity_ = _velocity; }
    void SetIsDrawCollisionArea(bool _isDraw) { isDrawCollisionArea_ = _isDraw; }


private:
    /// コールバック関数
    /// <summary>
    /// トリガー衝突時のコールバックです。
    /// </summary>
    /// <param name="_other">衝突相手のコライダー。</param>
    void OnCollisionTrigger(const Collider* _other);

    /// 内部関数
    /// <summary>
    /// 表示オブジェクト等の初期化を行います。
    /// </summary>
    void ObjectsInitialize();

    /// <summary>
    /// 表示オブジェクト等の更新を行います。
    /// </summary>
    void ObjectsUpdate();

    /// <summary>
    /// コライダーの初期化を行います。
    /// </summary>
    void CollidersInitialize();

    std::unique_ptr<TimeMeasurer> timer_ = nullptr;
    
    IModel* pModelSelfBody_ = nullptr;
    std::unique_ptr<Object3d> pBody_ = nullptr;
    
    /// パラメータ
    float lifeTimeLimit_ = 8.0f;
    Vector3 moveVelocity_ = {};

    /// コライダー
    std::unique_ptr<Collider> collider_ = nullptr;
    OBB obb_ = {};

    /// フラグ
    bool isDrawCollisionArea_ = false;


private: /// 他クラスの所有物
    CollisionManager* collisionManager_ = nullptr;
    DeltaTimeManager* deltaTimeManager_ = nullptr;
};
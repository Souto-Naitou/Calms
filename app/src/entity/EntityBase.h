#pragma once

#include <Vector3.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Lighting/PointLight/PointLight.h>
#include <Entity/Status/EntityStats.h>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <Interfaces/IEntityStats.h>
#include <memory>
#include <Common/structs.h>

struct EntityCommonParams
{
    DirectionalLight*   pDirLight   = nullptr;
    PointLight*         pPointLight = nullptr;
};

/// <summary>
/// エンティティ基底クラス
/// </summary>
class EntityBase
{
public:
    EntityBase() = default;
    virtual ~EntityBase() = default;

    /// <summary>
    /// エンティティの初期化を行います。
    /// デバッグウィンドウの有効化設定も同時に行われます。
    /// </summary>
    /// <param name="enableDebugWindow">デバッグウィンドウを有効にする場合は true。</param>
    virtual void Initialize(const EntityCommonParams& params, bool enableDebugWindow = true);

    /// <summary>
    /// エンティティの終了処理を行います。
    /// リソースの解放などを想定しています。
    /// </summary>
    virtual void Finalize() {};

    /// <summary>
    /// エンティティの状態更新を行います。
    /// 物理やアニメーション、内部タイミングの更新を想定しています。
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// エンティティの描画処理を行います。
    /// </summary>
    virtual void Draw() = 0;

    /// <summary>
    /// ライン等の補助的な描画処理を行います。
    /// デバッグ用のワイヤー表示などを想定しています。
    /// </summary>
    virtual void DrawLine() = 0;

    /// <summary>
    /// ImGui を用いたデバッグUIの描画を行います。
    /// </summary>
    virtual void ImGui();


public: /// Getter
    const Vector3& GetRotation() const { return rotation_; }
    const Vector3& GetScale() const { return scale_; }
    const Vector3& GetTranslation() const { return translation_; }
    const Vector3& GetVelocity() const { return velocity_; }
    const Vector3& GetAcceleration() const { return acceleration_; }
    float GetFriction() const { return friction_; }
    const bool IsAlive() const { return isAlive_; }
    const float GetAttackPower() const { return attackPower_; }
    const IEntityStats* GetStats() const { return &stats_; }


public: /// Setter
    void SetRotation(const Vector3& rotation) { rotation_ = rotation; }
    void SetScale(const Vector3& scale) { scale_ = scale; }
    void SetTranslation(const Vector3& translation) { translation_ = translation; }
    void SetVelocity(const Vector3& velocity) { velocity_ = velocity; }
    void SetAcceleration(const Vector3& acceleration) { acceleration_ = acceleration; }
    void SetFriction(float friction) { friction_ = friction; }
    void SetParams(EntityCommonParams& params) { commonParams_ = params; }


protected:
    std::unique_ptr<DebugEntry<EntityBase>> pDebugEntry_ = {};

    EntityCommonParams commonParams_    = {};

    bool    isEnableDebugWindow_        = true;
    bool    isAlive_                    = true;

    EntityStats  stats_                 = {};

    Vector3 rotation_                   = {};
    Vector3 scale_                      = {};
    Vector3 translation_                = {};
    Vector3 velocity_                   = {};
    Vector3 acceleration_               = {};

    float   friction_                   = 1.0f;
    float   attackPower_                = 0.0f;


protected:
    /// <summary>
    /// 単純な物理更新を行います。
    /// 加速度・速度・減衰（摩擦）を考慮して位置を更新します。
    /// </summary>
    /// <param name="dt">経過時間（秒）。</param>
    void UpdatePhysics(float dt);


protected: /// 他クラスの所有物
    GameEye** ppGameEye_ = nullptr;
};
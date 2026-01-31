#pragma once

#include <Vector3.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Lighting/PointLight/PointLight.h>
#include <Entity/Status/EntityStats.h>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <Interfaces/IEntityStats.h>
#include <memory>
#include <Common/structs.h>
#include <Features/Layer/Canvas.h>

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
    virtual void Initialize(bool enableDebugWindow = true);

    /// <summary>
    /// エンティティの終了処理を行います。
    /// リソースの解放などを想定しています。
    /// </summary>
    virtual void Finalize() = 0;

    /// <summary>
    /// エンティティの状態更新を行います。
    /// 物理やアニメーション、内部タイミングの更新を想定しています。
    /// </summary>
    virtual void Update() = 0;

    /// <summary>
    /// エンティティの描画処理を行います。
    /// </summary>
    virtual void Draw1F() = 0;

    /// <summary>
    /// ImGui を用いたデバッグUIの描画を行います。
    /// </summary>
    virtual void ImGui();

    /// Getter
    const bool IsAlive() const { return isAlive_; }

    /// Setter
    void SetName(const std::string& name);
    void SetGameEye(GameEye** ppGameEye) { ppGameEye_ = ppGameEye; }

protected:
    /// <summary>
    /// 生存フラグを false に設定します。
    /// </summary>
    void Dead() { isAlive_ = false; }
    void ShakeCamera(float power);

private:
    std::unique_ptr<DebugEntry<EntityBase>> pDebugEntry_ = {};
    bool        isAlive_    = true;
    GameEye**   ppGameEye_ = nullptr;
};
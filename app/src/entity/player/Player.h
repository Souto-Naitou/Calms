#pragma once

#include <Entity/EntityBase.h>
#include <drawable/object3d/Object3d.h>
#include <memory>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <Features/Audio/AudioManager.h>
#include <Features/Audio/Audio.h>
#include <Features/Model/ModelManager.h>
#include <Features/Model/IModel.h>
#include "PlayerInput.h"
#include "PlayerContext.h"
#include "PlayerMovement.h"
#include "PlayerExplosionTrigger.h"
#include <Math/Transform.h>
#include <entity/status/EntityStats.h>
#include <Features/Lighting/PointLight.h>
#include <Common/structs.h>
#include <Features/Primitive/AABB.h>
#include <component/MovementLimitterAABB.h>
#include <component/FocusOrientation.h>


/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public EntityBase
{
public:
    struct Params
    {
        ModelManager*       pModelManager   = nullptr;
        DirectionalLight*   pDirLight       = nullptr;
        PointLight*         pPointLight     = nullptr;
        AABB*               pMovableBounds  = nullptr;
        const Vector3*      pCursorPosition = nullptr;
    };

    enum class Flags : uint32_t
    {
        None            = 0x00000000,
        DisableMovement = 0x00000001,
        DisableInput    = 0x00000002,
        DisableRotation = 0x00000004,
    };

    Player(const Params& params);

    /// <summary>
    /// プレイヤーの初期化を行います。
    /// 必要なモデルやコライダー、入出力の初期設定を行います。
    /// </summary>
    /// <param name="enableDebugWindow">デバッグウィンドウの有効/無効。</param>
    void Initialize(bool enableDebugWindow = true) override;

    /// <summary>
    /// プレイヤーの終了処理を行います。
    /// リソースの解放などを想定しています。
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// プレイヤーの状態更新を行います。
    /// 入力処理、移動、当たり判定などの更新を行います。
    /// </summary>
    void Update() override;

    /// <summary>
    /// プレイヤーの描画処理を行います。
    /// </summary>
    void Draw1F() override;

    /// <summary>
    /// ImGui を用いたデバッグUIの描画を行います。
    /// </summary>
    void ImGui() override;


public: /// Getter
            bool            IsShot()        const   { return isShot_; }
            bool            IsSlow()        const   { return pInput_->GetData().isSlowPressed; }
            Object3d*       GetObject3d()   const   { return pObject_.get(); }
    const   PlayerContext&  GetContext()    const   { return *pContext_.get(); }
    const   EulerTransform& GetTransform()  const   { return transform_; }
    const   EntityStats&    GetStats()      const   { return *pStats_.get(); }

public: /// Setter
    void DisableMovement();
    void DisableInput();

private:
    /// <summary>
    /// トリガー型の衝突イベント時に呼ばれます。
    /// </summary>
    /// <param name="other">衝突相手のコライダー。</param>
    void OnCollisionTrigger(const Collider* other);

    /// <summary>
    /// 物理衝突時に呼ばれます。
    /// </summary>
    /// <param name="other">衝突相手のコライダー。</param>
    void OnCollision(const Collider* other);

    /// <summary>
    /// モデルや描画オブジェクトなどプレイヤー関連オブジェクトを初期化します。
    /// </summary>
    void ObjectsInitialize();

    /// <summary>
    /// コライダーの生成と設定を行います。
    /// </summary>
    void ColliderInitialize();

    /// <summary>
    /// 入力状態から移動や射撃などのコマンドを更新します。
    /// </summary>
    void UpdateInputCommands();

    /// <summary>
    /// オーディオハンドルの初期化を行います。
    /// </summary>
    void AudioHandleInitialize();

    void ComponentInitialize();

    static constexpr float  kGameEyeShakePowerWhenDamage_   = 0.3f;
    static constexpr float  kMovePower_                     = 25.0f;
    static constexpr float  kFriction_                      = 0.95f;
    static constexpr float  kReflectionPower_               = 70.0f;
    static constexpr float  kShotInterval_                  = 0.05f;
    static constexpr float  kLightIntensityDecreaseAmount_  = 1.0f;
    // 初期化パラメータ
    Params      params_;
    uint32_t    flags_      = static_cast<uint32_t>(Flags::None);

    /// [ コンポーネント ]
    EulerTransform                              transform_          = {};
    std::unique_ptr<PlayerInput>                pInput_             = nullptr;
    std::unique_ptr<PlayerContext>              pContext_           = nullptr;
    std::unique_ptr<PlayerMovement>             pMovement_          = nullptr;
    std::unique_ptr<PlayerExplosionTrigger>     pExplosionTrigger_  = nullptr;
    std::unique_ptr<FocusOrientation>           pFocusOrientation_  = nullptr;
    std::unique_ptr<EntityStats>                pStats_             = nullptr;
    std::unique_ptr<MovementLimitterAABB>       pAABBLimitter_      = nullptr;
    std::unique_ptr<IModel>                     pModelSelfBody_     = nullptr;
    std::unique_ptr<Object3d>                   pObject_            = nullptr;
    std::unique_ptr<Collider>                   pCollider_          = nullptr;
    std::unique_ptr<TimeMeasurer>               pTimerShot_         = nullptr;

    bool  isShot_   = false;

    /// コライダー用
    OBB obb_ = {};

    /// [ オーディオ ]
    Audio*  pAudioShot_     = nullptr;
    Audio*  pAudioDeath_    = nullptr;
    Audio*  pAudioSlowOn_   = nullptr;
    Audio*  pAudioSlowOff_  = nullptr;


private: /// 他クラスの所有物
    CollisionManager*   pCollisionManager_  = nullptr;
    DeltaTimeManager*   pDeltaTimeManager_  = nullptr;
    AudioManager*       pAudioManager_      = nullptr;
    ModelManager*       pModelManager_      = nullptr;
};
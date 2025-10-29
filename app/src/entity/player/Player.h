#pragma once

#include <Entity/EntityBase.h>
#include <Features/Object3d/Object3d.h>
#include <memory>
#include <Features/Input/Input.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <Features/Collision/Collider/Collider.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/Particle/Emitter/ParticleEmitter.h>
#include <Features/Audio/AudioManager.h>
#include <Features/Audio/Audio.h>
#include <Features/Model/ModelManager.h>
#include <Features/Model/IModel.h>

/// <summary>
/// プレイヤークラス
/// </summary>
class Player : public EntityBase
{
public:
    Player(ModelManager* pModelManager)
        : pModelManager_(pModelManager) {};

    /// <summary>
    /// プレイヤーの初期化を行います。
    /// 必要なモデルやコライダー、入出力の初期設定を行います。
    /// </summary>
    /// <param name="enableDebugWindow">デバッグウィンドウの有効/無効。</param>
    void Initialize(const EntityCommonParams& params, bool enableDebugWindow = true) override;

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
    void Draw() override;

    /// <summary>
    /// プレイヤーのライン等の補助描画を行います。
    /// </summary>
    void DrawLine() override;

    /// <summary>
    /// ImGui を用いたデバッグUIの描画を行います。
    /// </summary>
    void ImGui() override;


public: /// Getter
    bool IsShot() const { return isShot_; }
    bool IsSlow() const { return isSlow_; }
    Object3d* GetObject3d() const { return object_.get(); }

public: /// Setter
    void SetIsDrawCollisionArea(bool isDraw) { isDrawCollisionArea_ = isDraw; }
    void SetEnableInput(bool enable) { enableInput_ = enable; }


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

    std::unique_ptr<IModel>         pModelSelfBody_ = nullptr;
    std::unique_ptr<Object3d>       object_ = {};
    std::unique_ptr<TimeMeasurer>   timerShot_ = {};
    float movePower_ = 0.0f;

    /// 射撃
    bool isShot_ = false;
    double shotInterval_ = 0.05;

    /// スロー
    bool isSlow_ = false;

    /// コライダー
    std::unique_ptr<Collider> collider_ = nullptr;
    OBB obb_ = {};
    bool isDrawCollisionArea_ = false;

    /// 反発用
    Vector3 accelerationRefl_ = {};
    float reflectionPower_ = 70.0f;

    /// マルチプレイ用
    bool enableInput_ = true;

    /// パーティクルエミッター
    std::unique_ptr<ParticleEmitter> shotEmitter_ = nullptr;
    std::shared_ptr<IModel> pModelSpark_ = nullptr;

    /// オーディオ
    Audio* audioShot_ = nullptr;


private: /// 他クラスの所有物
    Input* input_ = nullptr;
    CollisionManager* collisionManager_ = nullptr;
    DeltaTimeManager* deltaTimeManager_ = nullptr;
    AudioManager* audioManager_ = nullptr;
    ModelManager* pModelManager_ = nullptr;
};
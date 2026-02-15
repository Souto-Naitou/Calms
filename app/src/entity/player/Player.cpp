#include "Player.h"

#include <imgui.h>
#include <Features/Model/ObjModel.h>
#include <config/ResourcePath.h>

Player::Player(const Params& params) : pModelManager_(params.pModelManager)
{
    params_ = params;
}

void Player::Initialize(bool enableDebugWindow)
{
    /// [ 基底クラスの初期化 ]
    EntityBase::Initialize(enableDebugWindow);

    /// [ デバッグ機能の初期化 ]
    this->SetName("Player");

    /// [ インスタンスの取得 ]
    pCollisionManager_ = CollisionManager::GetInstance();
    pDeltaTimeManager_ = DeltaTimeManager::GetInstance();
    pAudioManager_ = AudioManager::GetInstance();

    /// [ コンポーネントの初期化 ]
    this->ComponentInitialize();

    /// [ タイマーの初期化 ]
    pTimerShot_ = std::make_unique<TimeMeasurer>();
    pTimerShot_->Start();

    /// [ パーティクルエミッターの初期化 ]
    this->ParticleEmittersInitialize();

    // オブジェクトの初期化
    this->ObjectsInitialize();

    // OBBの初期化
    obb_.Initialize();

    // コライダーの初期化
    this->ColliderInitialize();

    // オーディオハンドルの初期化
    this->AudioHandleInitialize();

    if (params_.pDirLight) pObject_->SetDirectionalLight(params_.pDirLight);
    if (params_.pPointLight) pObject_->SetPointLight(params_.pPointLight);
}

void Player::Finalize()
{
    pObject_->Finalize();
    pCollisionManager_->UnregisterCollider(pCollider_.get());
    if (pEmitterConstant_) pEmitterConstant_->Finalize();
}

void Player::Update()
{
    const auto  kDeltaTimeChannel = static_cast<uint32_t>(DeltaTimeChannelReserved::Game);
    const float kDeltaTime = pDeltaTimeManager_->GetDeltaTime(kDeltaTimeChannel);

    isShot_ = false;

    // 入力コマンドの更新
    if (!(flags_ & static_cast<uint32_t>(Flags::DisableInput)))
    {
        pInput_->Update();
        /// #TODO: 関数内の処理を別クラスに分離する
        this->UpdateInputCommands();
    }

    /// 座標更新
    if (!(flags_ & static_cast<uint32_t>(Flags::DisableMovement)))
    {
        pMovement_->ApplyFriction(kFriction_);
        pMovement_->Update(transform_, kDeltaTime);
    }

    // AABBリミッターの更新 (ここで座標補正が入る)
    if (pAABBLimitter_) pAABBLimitter_->Update(transform_);

    // パーティクルエミッターの更新 (動いている or 動きが無効化されている場合にパーティクルをエミット)
    if (pEmitterConstant_)
    {
        if ((pMovement_->IsMove(0.2f)) || flags_ & static_cast<uint32_t>(Flags::DisableMovement))
        {

            pEmitterConstant_->SetPosition(transform_.translate);
            pEmitterConstant_->Emit();
        }
        pEmitterConstant_->Update();
    }

    pExplosionTrigger_->Update();

    /// 3dモデルの更新
    pObject_->SetTranslate(transform_.translate);
    pObject_->Update();

    /// コライダーの更新
    obb_.SetCenter(transform_.translate);
    obb_.SetOrientations(pObject_->GetRotateMatrix());
    obb_.SetSize(Vector3(0.5f, 0.5f, 0.5f));
    pCollider_->SetShapeData(&obb_);
}

void Player::Draw1F()
{
    // オブジェクトの描画
    pObject_->Draw1F();
    if (pEmitterConstant_) pEmitterConstant_->Draw1F();
}

void Player::ObjectsInitialize()
{
    /// オブジェクトの初期化
    auto originalModel = pModelManager_->Load(Path::Model::kPlayer);
    pModelSelfBody_ = originalModel->Cloned();
    pObject_ = std::make_unique<Object3d>();
    pObject_->Initialize();
    pObject_->SetName("player");
    pObject_->SetTranslate(Vector3(0, 0.5f, 0));
    pObject_->SetRotate(Vector3(0, 0, 0));
    pObject_->SetModel(pModelSelfBody_.get());
    auto& option = pObject_->GetOption();
    option.materialData->environmentCoefficient = 0.0f;
    option.materialData->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
}

void Player::ColliderInitialize()
{
    /// コライダーの初期化
    pCollider_ = std::make_unique<Collider>();
    pCollider_->SetColliderID("player");
    pCollider_->SetAttribute(pCollisionManager_->GetNewAttribute("player"));
    pCollider_->SetOwner(this);
    pCollider_->SetShape(Shape::OBB);
    pCollider_->SetMask(pCollisionManager_->GetNewMask("player"));
    pCollider_->SetOnCollision(std::bind(&Player::OnCollision, this, std::placeholders::_1));
    pCollider_->SetOnCollisionTrigger(std::bind(&Player::OnCollisionTrigger, this, std::placeholders::_1));
    pCollider_->SetOwnerTransform(&transform_);
    pCollider_->SetEntityStats(pStats_.get());
    // コライダーの登録
    pCollisionManager_->RegisterCollider(pCollider_.get());
}

void Player::ParticleEmittersInitialize()
{
    if (!params_.particle) return;
    ParticleEmitterInitParams emitterParams = {};
    emitterParams.particle = params_.particle;
    emitterParams.jsonPath = Path::ParticleEmitter::kPlayerConstantTrail;
    pEmitterConstant_ = std::make_unique<ParticleEmitter>();
    pEmitterConstant_->Initialize(emitterParams);
    pEmitterConstant_->EnableManualMode();
    pEmitterConstant_->SetEnableBillboard(true);
}

void Player::UpdateInputCommands()
{
    if (this->IsAlive() == false) return;

    const auto& inputData = pInput_->GetData();

    if (inputData.isSlowTriggered)
    {
        pAudioSlowOn_->Play();
    }
    else if (inputData.isSlowReleased)
    {
        pAudioSlowOff_->Play();
    }

    if (inputData.isShotPressed)
    {
        if (pTimerShot_->GetNow<float>() > kShotInterval_)
        {
            pAudioShot_->Play();
            isShot_ = true;
            pTimerShot_->Reset();
            pTimerShot_->Start();
        }
    }
}

void Player::AudioHandleInitialize()
{
    pAudioShot_ = pAudioManager_->GetNewAudio("Effect", Path::Audio::kSePlayerShoot);
    pAudioShot_->SetVolume(0.1f);
    pAudioDeath_ = pAudioManager_->GetNewAudio("Effect", Path::Audio::kSePlayerDeath);
    pAudioDeath_->SetVolume(0.15f);
    pAudioSlowOn_ = pAudioManager_->GetNewAudio("Effect", Path::Audio::kSePlayerSlowOn);
    pAudioSlowOn_->SetVolume(0.1f);
    pAudioSlowOff_ = pAudioManager_->GetNewAudio("Effect", Path::Audio::kSePlayerSlowOff);
    pAudioSlowOff_->SetVolume(0.1f);
}

void Player::ComponentInitialize()
{
    // トランスフォーム
    transform_.scale        = Vector3(1.0f, 1.0f, 1.0f);
    transform_.rotate       = Vector3(0.0f, 0.0f, 0.0f);
    transform_.translate    = Vector3(0, 0.5f, 0);
    // ステータス
    pStats_ = std::make_unique<EntityStats>();
    pStats_->Initialize(100.0f, 0.0f, 20.0f);
    // 入力
    pInput_ = std::make_unique<PlayerInput>();
    pInput_->Initialize();
    // コンテキスト
    pContext_ = std::make_unique<PlayerContext>();
    pContext_->Initialize();
    // 移動
    pMovement_ = std::make_unique<PlayerMovement>(pInput_.get());
    pMovement_->SetMovePower(kMovePower_);
    // 爆発トリガー
    pExplosionTrigger_ = std::make_unique<PlayerExplosionTrigger>();
    pExplosionTrigger_->Initialize(pInput_.get(), pContext_.get());
    // AABB制限
    if (params_.pMovableBounds)
    {
        pAABBLimitter_ = std::make_unique<MovementLimitterAABB>();
        pAABBLimitter_->SetBounds(params_.pMovableBounds);
    }
}

void Player::ImGui()
{
#ifdef _DEBUG
    EntityBase::ImGui();
    pExplosionTrigger_->ImGui();
#endif
}

void Player::DisableMovement()
{
    flags_ |= static_cast<uint32_t>(Flags::DisableMovement);
}

void Player::DisableInput()
{
    flags_ |= static_cast<uint32_t>(Flags::DisableInput);
}

void Player::OnCollisionTrigger(const Collider* other)
{
    if (other->GetColliderID() == "enemy")
    {
        auto pOtherEntityStats = other->GetEntityStats();
        assert(pOtherEntityStats);
        pStats_->OnCollision(pOtherEntityStats);
        if (params_.pDirLight) params_.pDirLight->intensity -= kLightIntensityDecreaseAmount_;
        EntityBase::ShakeCamera(kGameEyeShakePowerWhenDamage_);
        if (pStats_->GetHp() <= 0.0f)
        {
            EntityBase::Dead();
            pCollider_->SetEnable(false);
            pAudioDeath_->Play();
        }
    }
}

void Player::OnCollision(const Collider* other)
{
    const EntityBase* otherOwner = other->GetOwner<EntityBase>();

    if (other->GetColliderID() == "enemy")
    {
        assert(other->GetOwnerTransform() && "衝突相手のTransformが設定されていません。");
        /// 反発を速度に適用
        Vector3 otherPos = other->GetOwnerTransform()->translate;
        otherPos.y = transform_.translate.y; // Y軸は無視する
        Vector3 dir = transform_.translate - otherPos;

        if (pMovement_)
        {
            pMovement_->ApplyForce(dir * kReflectionPower_);
        }
    }
}

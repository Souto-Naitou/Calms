#include "Player.h"

#include <imgui.h>
#include <Features/Model/ObjModel.h>
#include <config/ResourcePath.h>

Player::Player(const Params& params) : pModelManager_(params.pModelManager)
{
    params_ = params;
}

void Player::Initialize(const EntityCommonParams& params, bool enableDebugWindow)
{
    /// [ 基底クラスの初期化 ]
    EntityBase::Initialize(params, enableDebugWindow);

    /// [ デバッグ機能の初期化 ]
    pDebugEntry_->SetName("Player");

    /// [ インスタンスの取得 ]
    collisionManager_ = CollisionManager::GetInstance();
    deltaTimeManager_ = DeltaTimeManager::GetInstance();
    audioManager_ = AudioManager::GetInstance();

    /// [ コンポーネントの初期化 ]
    this->ComponentInitialize();

    /// [ タイマーの初期化 ]
    timerShot_ = std::make_unique<TimeMeasurer>();
    timerShot_->Start();

    /// [ パーティクルエミッターの初期化 ]
    this->ParticleEmittersInitialize();

    /// [ パラメータの初期化 ]
    movePower_ = 25.0f;
    friction_ = 0.95f;
    transform_.translate = Vector3(0, 0.5f, 0);
    stats_.Initalize(100.0f, 0.0f, 20.0f);

    // オブジェクトの初期化
    this->ObjectsInitialize();

    // OBBの初期化
    obb_.Initialize();

    // コライダーの初期化
    this->ColliderInitialize();

    // オーディオハンドルの初期化
    this->AudioHandleInitialize();

    if (commonParams_.pDirLight) object_->SetDirectionalLight(commonParams_.pDirLight);
    if (commonParams_.pPointLight) object_->SetPointLight(commonParams_.pPointLight);
}

void Player::Finalize()
{
    object_->Finalize();
    emitterConstant_->Finalize();
    
    EntityBase::Finalize();
}

void Player::Update()
{
    const float kDeltaTime = deltaTimeManager_->GetDeltaTime(1);

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
        pMovement_->Update(kDeltaTime);
    }

    // パーティクルエミッターの更新
    if ((pMovement_->GetData().velocity.Length() > 0.2f ) || flags_ & static_cast<uint32_t>(Flags::DisableMovement))
    {
        emitterConstant_->SetPosition(transform_.translate);
        emitterConstant_->Emit();
    }
    emitterConstant_->Update();

    pExplosionTrigger_->Update();

    /// 3dモデルの更新
    object_->SetTranslate(transform_.translate);
    object_->Update();

    /// コライダーの更新
    obb_.SetCenter(transform_.translate);
    obb_.SetOrientations(object_->GetRotateMatrix());
    obb_.SetSize(Vector3(0.5f, 0.5f, 0.5f));
    collider_->SetShapeData(&obb_);
}

void Player::Draw1F()
{
    // オブジェクトの描画
    object_->Draw1F();
}

void Player::DrawLine()
{
    // パーティクルエミッターの描画
    emitterConstant_->Draw();
}

void Player::ObjectsInitialize()
{
    /// オブジェクトの初期化
    auto originalModel = pModelManager_->Load("Cube/Cube.obj");
    pModelSelfBody_ = originalModel->Cloned();
    object_ = std::make_unique<Object3d>();
    object_->Initialize();
    object_->SetName("player");
    object_->SetTranslate(Vector3(0, 0.5f, 0));
    object_->SetRotate(Vector3(0, 0, 0));
    object_->SetModel(pModelSelfBody_.get());
    auto& option = object_->GetOption();
    option.materialData->environmentCoefficient = 0.0f;
    option.materialData->color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
}

void Player::ColliderInitialize()
{
    /// コライダーの初期化
    collider_ = std::make_unique<Collider>();
    collider_->SetColliderID("player");
    collider_->SetAttribute(collisionManager_->GetNewAttribute("player"));
    collider_->SetOwner(this);
    collider_->SetShape(Shape::OBB);
    collider_->SetRadius(2u);
    collider_->SetMask(collisionManager_->GetNewMask("player"));
    collider_->SetOnCollision(std::bind(&Player::OnCollision, this, std::placeholders::_1));
    collider_->SetOnCollisionTrigger(std::bind(&Player::OnCollisionTrigger, this, std::placeholders::_1));
    collider_->SetEnableLighter(true);
    // コライダーの登録
    collisionManager_->RegisterCollider(collider_.get());
}

void Player::ParticleEmittersInitialize()
{
    ParticleEmitterInitParams emitterParams = {};
    emitterParams.particle = params_.particle;
    emitterParams.jsonPath = "resources/json/particles/PlayerConstant.json";
    emitterConstant_ = std::make_unique<ParticleEmitter>();
    emitterConstant_->Initialize(emitterParams);
    emitterConstant_->EnableManualMode();
    emitterConstant_->SetEnableBillboard(true);
}

void Player::UpdateInputCommands()
{
    if (isAlive_ == false) return;

    const auto& inputData = pInput_->GetData();

    if (inputData.isSlowTriggered)
    {
        audioSlowOn_->Play();
    }
    else if (inputData.isSlowReleased)
    {
        audioSlowOff_->Play();
    }

    if (inputData.isShotPressed)
    {
        if (timerShot_->GetNow<float>() > shotInterval_)
        {
            audioShot_->Play();
            isShot_ = true;
            timerShot_->Reset();
            timerShot_->Start();
        }
    }
}

void Player::AudioHandleInitialize()
{
    audioShot_ = audioManager_->GetNewAudio("Effect", Path::Audio::kSePlayerShoot);
    audioShot_->SetVolume(0.1f);
    audioDeath_ = audioManager_->GetNewAudio("Effect", Path::Audio::kSePlayerDeath);
    audioDeath_->SetVolume(0.15f);
    audioSlowOn_ = audioManager_->GetNewAudio("Effect", Path::Audio::kSePlayerSlowOn);
    audioSlowOn_->SetVolume(0.1f);
    audioSlowOff_ = audioManager_->GetNewAudio("Effect", Path::Audio::kSePlayerSlowOff);
    audioSlowOff_->SetVolume(0.1f);
}

void Player::ComponentInitialize()
{
    // 入力
    pInput_ = std::make_unique<PlayerInput>();
    pInput_->Initialize();
    // 移動
    pMovement_ = std::make_unique<PlayerMovement>();
    pMovement_->Initialize(pInput_.get(), &transform_);
    // 爆発トリガー
    pExplosionTrigger_ = std::make_unique<PlayerExplosionTrigger>();
    pExplosionTrigger_->Initialize(pInput_.get());
}

void Player::ImGui()
{
#ifdef _DEBUG
    EntityBase::ImGui();
    ImGui::DragFloat("MovePower", &movePower_, 0.12f);

    pExplosionTrigger_->ImGui();
#endif
}

void Player::DisableMovement()
{
    pMovement_->SetEnable(false);
    flags_ |= static_cast<uint32_t>(Flags::DisableMovement);
}

void Player::DisableInput()
{
    flags_ |= static_cast<uint32_t>(Flags::DisableInput);
}

void Player::OnCollisionTrigger(const Collider* other)
{
    const EntityBase* otherOwner = other->GetOwner<EntityBase>();

    if (other->GetColliderID() == "enemy")
    {
        stats_.OnCollision(other->GetOwner<EntityBase>()->GetStats());

        commonParams_.pDirLight->intensity -= 1.0f;

        if (stats_.GetHp() <= 0.0f)
        {
            isAlive_ = false;
            collider_->SetEnable(false);
            audioDeath_->Play();
        }
    }
}

void Player::OnCollision(const Collider* other)
{
    const EntityBase* otherOwner = other->GetOwner<EntityBase>();

    if (other->GetColliderID() == "enemy")
    {
        /// 反発を速度に適用
        Vector3 otherPos = otherOwner->GetTranslation();
        Vector3 dir = transform_.translate - otherPos;

        if (pMovement_)
        {
            pMovement_->AddAcceleration(dir * reflectionPower_);
        }
    }
}

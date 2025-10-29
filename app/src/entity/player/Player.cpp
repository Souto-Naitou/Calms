#include "Player.h"

#include <imgui.h>
#include <Features/Model/ObjModel.h>

void Player::Initialize(const EntityCommonParams& params, bool enableDebugWindow)
{
    // 基底クラスの初期化
    EntityBase::Initialize(params, enableDebugWindow);
    pDebugEntry_->SetName("Player");


    /// インスタンスの取得
    input_ = Input::GetInstance();
    collisionManager_ = CollisionManager::GetInstance();
    deltaTimeManager_ = DeltaTimeManager::GetInstance();
    audioManager_ = AudioManager::GetInstance();

    
    /// タイマーの初期化
    timerShot_ = std::make_unique<TimeMeasurer>();
    timerShot_->Start();


    /// パラメータの初期化
    movePower_ = 20.0f;
    friction_ = 0.95f;
    translation_ = Vector3(0, 0.5f, 0);
    stats_.Initalize(100.0f, 0.0f, 20.0f);

    // オブジェクトの初期化
    this->ObjectsInitialize();

    /// OBBの初期化
    obb_.Initialize();

    // コライダーの初期化
    this->ColliderInitialize();

    // コライダーの登録
    collisionManager_->RegisterCollider(collider_.get());

    pModelSpark_ = std::make_unique<ObjModel>();
    pModelSpark_->Clone(pModelManager_->Load("Particle/ParticleSpark.obj"));

    /// パーティクルエミッターの初期化
    shotEmitter_ = std::make_unique<ParticleEmitter>();
    shotEmitter_->Initialize(pModelSpark_.get(), "resources/json/Spark.json");
    shotEmitter_->SetEnableBillboard(true);

    audioShot_ = audioManager_->GetNewAudio("Effect", "hit_hat.wav");
    audioShot_->SetVolume(0.1f);

    if (commonParams_.pDirLight) object_->SetDirectionalLight(commonParams_.pDirLight);
    if (commonParams_.pPointLight) object_->SetPointLight(commonParams_.pPointLight);
}


void Player::Finalize()
{
    object_->Finalize();
    shotEmitter_->Finalize();
    
    EntityBase::Finalize();
}


void Player::Update()
{
    // 入力コマンドの更新
    if (enableInput_) UpdateInputCommands();

    /// 反発の速度を適用
    acceleration_ += accelerationRefl_;
    accelerationRefl_ = Vector3(0, 0, 0);

    // 座標更新
    EntityBase::UpdatePhysics(deltaTimeManager_->GetDeltaTime(1));

    // 座標の反映
    object_->SetTranslate(translation_);

    // オブジェクトの更新
    object_->Update();

    /// コライダーの更新
    obb_.SetCenter(translation_);
    obb_.SetOrientations(object_->GetRotateMatrix());
    obb_.SetSize(Vector3(0.5f, 0.5f, 0.5f));

    collider_->SetShapeData(&obb_);

    /// パーティクルエミッターの更新
    shotEmitter_->SetPosition(translation_);
    shotEmitter_->Update();
}


void Player::Draw()
{
    // オブジェクトの描画
    object_->Draw();
}

void Player::DrawLine()
{
    if (isDrawCollisionArea_) collider_->DrawArea();
    // パーティクルエミッターの描画
    shotEmitter_->Draw();
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
}

void Player::UpdateInputCommands()
{
    if (input_->PushKey(DIK_W))
    {
        acceleration_.z = movePower_;
    }
    if (input_->PushKey(DIK_S))
    {
        acceleration_.z = -movePower_;
    }
    if (input_->PushKey(DIK_A))
    {
        acceleration_.x = -movePower_;
    }
    if (input_->PushKey(DIK_D))
    {
        acceleration_.x = movePower_;
    }

    isShot_ = false;
    if (input_->PushMouse(Input::MouseNum::Left))
    {
        if (timerShot_->GetNow<float>() > shotInterval_)
        {
            audioShot_->Play();
            isShot_ = true;
            timerShot_->Reset();
            timerShot_->Start();
        }
        shotEmitter_->Emit();
    }

    isSlow_ = false;
    if (input_->PushKey(DIK_LSHIFT))
    {
        isSlow_ = true;
    }
}

void Player::ImGui()
{
#ifdef _DEBUG
    EntityBase::ImGui();
    ImGui::DragFloat("MovePower", &movePower_, 0.12f);

    ImGui::SeparatorText("Debug");
    ImGui::Checkbox("Draw2D Collision Area", &isDrawCollisionArea_);
#endif
}

void Player::OnCollisionTrigger(const Collider* other)
{
    const EntityBase* otherOwner = other->GetOwner<EntityBase>();

    if (other->GetColliderID() == "enemy")
    {
        stats_.OnCollision(other->GetOwner<EntityBase>()->GetStats());
    }
}

void Player::OnCollision(const Collider* other)
{
    const EntityBase* otherOwner = other->GetOwner<EntityBase>();

    if (other->GetColliderID() == "enemy")
    {
        /// 反発を速度に適用
        Vector3 otherPos = otherOwner->GetTranslation();
        Vector3 dir = translation_ - otherPos;

        accelerationRefl_ = dir * reflectionPower_;
    }
}

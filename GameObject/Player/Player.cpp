#include "Player.h"

#include <imgui.h>

void Player::Initialize()
{
    // 基底クラスの初期化
    BaseObject::Initialize();


    /// インスタンスの取得
    input_ = Input::GetInstance();
    collisionManager_ = CollisionManager::GetInstance();
    deltaTimeManager_ = DeltaTimeManager::GetInstance();

    
    /// タイマーの初期化
    timerShot_ = std::make_unique<Timer>();


    /// パラメータの初期化
    name_ = "player";
    movePower_ = 20.0f;
    friction_ = 0.95f;
    translation_ = Vector3(0, 0.5f, 0);
    hp_ = 100.0f;


    /// オブジェクトの初期化
    object_ = std::make_unique<Object3d>();
    object_->Initialize("Cube.obj");
    object_->SetName("player");
    object_->SetTranslate(Vector3(0, 0.5f, 0));
    object_->SetRotate(Vector3(0, 0, 0));
    object_->SetColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f));


    /// OBBの初期化
    obb_.Initialize();


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

    /// パーティクルエミッターの初期化
    shotEmitter = std::make_unique<ParticleEmitter>();
    shotEmitter->Initialize("Particle/ParticleSpark.obj", "resources/json/particles/shot.json");
    shotEmitter->SetEnableBillboard(true);
}


void Player::Finalize()
{
    object_->Finalize();
    shotEmitter->Finalize();

    object_.reset();
    shotEmitter.reset();
    
    BaseObject::Finalize();
}


void Player::Update()
{
    // 入力コマンドの更新
    if (enableInput_) UpdateInputCommands();

    /// 反発の速度を適用
    acceleration_ += accelerationRefl_;
    accelerationRefl_ = Vector3(0, 0, 0);

    // 座標更新
    BaseObject::UpdateTransform(deltaTimeManager_->GetDeltaTime(1));

    // 座標の反映
    object_->SetTranslate(translation_);

    // オブジェクトの更新
    object_->Update();
    if (!directionalLight_)
    {
        directionalLight_ = diContainer_->Resolve<DirectionalLight>();
        object_->SetDirectionalLight(directionalLight_);
    }

    if (!pointLight_)
    {
        pointLight_ = diContainer_->Resolve<PointLight>();
        object_->SetPointLight(pointLight_);
    }

    /// コライダーの更新
    obb_.SetCenter(translation_);
    obb_.SetOrientations(object_->GetRotateMatrix());
    obb_.SetSize(Vector3(0.5f, 0.5f, 0.5f));

    collider_->SetShapeData(&obb_);

    /// パーティクルエミッターの更新
    shotEmitter->SetPosition(translation_);
    shotEmitter->Update();
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
    shotEmitter->Draw();
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
        if (timerShot_->GetNow() > shotInterval_)
        {
            isShot_ = true;
            timerShot_->Reset();
            timerShot_->Start();
        }
        shotEmitter->Emit();
    }

    isSlow_ = false;
    if (input_->PushKey(DIK_LSHIFT))
    {
        isSlow_ = true;
    }
}

void Player::DebugWindow()
{
#ifdef _DEBUG
    BaseObject::DebugWindow();
    ImGui::DragFloat("MovePower", &movePower_, 0.12f);

    ImGui::SeparatorText("Debug");
    ImGui::Checkbox("Draw2D Collision Area", &isDrawCollisionArea_);
#endif
}

void Player::ModifyGameEye(GameEye* _eye)
{
    object_->SetGameEye(gameEye_);
    obb_.SetGameEye(gameEye_);
    shotEmitter->SetGameEye(gameEye_);
}

void Player::OnCollisionTrigger(const Collider* _collider)
{
    if (_collider->GetColliderID() == "enemy")
    {
        hp_ -= _collider->GetOwner()->GetAttackPower();
    }
}

void Player::OnCollision(const Collider* _other)
{
    if (_other->GetColliderID() == "enemy")
    {
        /// 反発を速度に適用
        Vector3 otherPos = _other->GetOwner()->GetTranslation();
        Vector3 dir = translation_ - otherPos;

        accelerationRefl_ = dir * reflectionPower_;
    }
}

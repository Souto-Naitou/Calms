#include "Player.h"

#include <imgui.h>

void Player::Initialize()
{
    // 基底クラスの初期化
    BaseObject::Initialize();


    /// インスタンスの取得
    input_ = Input::GetInstance();
    collisionManager_ = CollisionManager::GetInstance();

    
    /// タイマーの初期化
    timerShot_ = std::make_unique<Timer>();


    /// パラメータの初期化
    name_ = "player";
    movePower_ = 20.0f;
    friction_ = 0.95f;
    translation_ = Vector3(0, 0.5f, 0);


    /// オブジェクトの初期化
    object_ = std::make_unique<Object3d>();
    object_->Initialize("Cube.obj");
    object_->SetName("player");
    object_->SetTranslate(Vector3(0, 0.5f, 0));
    object_->SetRotate(Vector3(0, 0, 0));


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

    // コライダーの登録
    collisionManager_->RegisterCollider(collider_.get());
}


void Player::Finalize()
{
    object_->Finalize();
    object_.reset();
    BaseObject::Finalize();
}


void Player::Update()
{
    // 入力コマンドの更新
    UpdateInputCommands();

    // 座標更新
    BaseObject::UpdateTransform();

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
}


void Player::Draw()
{
    // オブジェクトの描画
    object_->Draw();
}

void Player::DrawLine()
{
    if (isDrawCollisionArea_) collider_->DrawArea();
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
    if (input_->PushKey(DIK_SPACE))
    {
        if (timerShot_->GetNow() > shotInterval_)
        {
            isShot_ = true;
            timerShot_->Reset();
            timerShot_->Start();
        }
    }
}

void Player::DebugWindow()
{
    BaseObject::DebugWindow();
    ImGui::DragFloat("MovePower", &movePower_, 0.12f);

    ImGui::SeparatorText("Debug");
    ImGui::Checkbox("Draw2D Collision Area", &isDrawCollisionArea_);
}

void Player::ModifyGameEye(GameEye* _eye)
{
    object_->SetGameEye(gameEye_);
    obb_.SetGameEye(gameEye_);
}

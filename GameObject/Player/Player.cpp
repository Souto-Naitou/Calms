#include "Player.h"

#include <imgui.h>

void Player::Initialize()
{
    // 基底クラスの初期化
    BaseObject::Initialize();


    /// インスタンスの取得
    input_ = Input::GetInstance();

    
    /// タイマーの初期化
    timerShot_ = std::make_unique<Timer>();


    /// パラメータの初期化
    name_ = "player";
    movePower_ = 20.0f;
    friction_ = 0.95f;


    /// オブジェクトの初期化
    object_ = std::make_unique<Object3d>();
    object_->SetName("player");
    object_->Initialize("Cube.obj");
    object_->SetTranslate(Vector3(0, 0.5f, 0));
    object_->SetRotate(Vector3(0, 0, 0));
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
}


void Player::Draw()
{
    // オブジェクトの描画
    object_->Draw();
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
}

void Player::ModifyGameEye(GameEye* _eye)
{
    object_->SetGameEye(gameEye_);
}

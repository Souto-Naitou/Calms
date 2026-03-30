#include "Calms.h"

#include <Common/define.h>
#include <Features/SceneManager/SceneManager.h>
#include <scene/factory/SceneFactory.h>
#include <Features/Model/Helper/ModelHelper.h>
#include <NiGui.h>
#include <config/ResourcePath.h>

#pragma comment(lib, "Ws2_32.lib")

void Calms::Initialize()
{
    /// 基底クラスの初期化処理
    NimaFramework::Initialize();

    #ifdef _DEBUG
    pImGuiManager_->EnableDocking();
    #endif
    
    /// シーンファクトリの設定
    pSceneFactory_ = std::make_unique<SceneFactory>();
    pSceneManager_->SetSceneFactory(pSceneFactory_.get());
    pIntermediateScreenFactory_ = std::make_unique<IntermediateScreenFactory>();
    pTransitionExecutor_->SetIntermediateScreenFactory(pIntermediateScreenFactory_.get());

    /// 当たり判定マネージャの初期化
    pCollisionManager_ = CollisionManager::GetInstance();
    pCollisionManager_->Initialize();

    /// モデルローダー、ストレージ、マネージャの初期化
    pModelLoader_ = Helper::Model::CreateLoader<ModelLoaderAssimp>(pDirectX_.get());
    pModelStorage_ = Helper::Model::CreateStorage();
    pModelManager_ = Helper::Model::CreateManager(pModelLoader_.get(), pModelStorage_.get());

    // モデルマネージャをシーンマネージャにセット
    pSceneManager_->SetModelManager(pModelManager_.get());

    /// 入力マッパーの初期化
    this->InitializeInputMapper();

    /// Dissolve用のテクスチャをロード
    pTextureManager_->LoadTexture("noise0.png");

    /// UIサウンドの設定
    auto pAudioConfirm = pAudioManager_->GetNewAudio("UI", Path::Audio::UI::kConfirm);
    pAudioConfirm->SetVolume(0.5f);
    NiGui::SetConfirmSound(pAudioConfirm);
    NiGui::SetHoverSound(pAudioManager_->GetNewAudio("UI", Path::Audio::UI::kHover));
}

void Calms::Finalize()
{
    /// 基底クラスの終了処理
    NimaFramework::Finalize();
}

void Calms::Update()
{
    /// 当たり判定の更新
    pCollisionManager_->CheckAllCollision();

    /// 基底クラスの更新処理
    NimaFramework::Update();
}

void Calms::Draw()
{
    /// 描画前処理
    NimaFramework::PreProcess();


    /// バックバッファ書き込み
    NimaFramework::Draw();


    /// 描画後処理
    NimaFramework::PostProcess();
}

void Calms::InitializeInputMapper()
{
    pInputMapperUI_ = std::make_unique<InputMapper<InputActionUI>>();
    pInputMapperUI_->MapAction(InputActionUI::Confirm, { InputMapper<InputActionUI>::KeyCode{ DIK_SPACE } });
    pInputMapperUI_->MapAction(InputActionUI::Confirm, { InputMapper<InputActionUI>::PadButton{ XINPUT_GAMEPAD_A } });

    pSceneManager_->AddInitialArg("InputMapperUI", pInputMapperUI_.get());
}

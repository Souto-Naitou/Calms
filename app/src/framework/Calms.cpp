#include "Calms.h"

#include <Common/define.h>
#include <Features/SceneManager/SceneManager.h>
#include <Scene/Factory/SceneFactory.h>
#include <Features/Model/Helper/ModelHelper.h>

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

    /// 当たり判定マネージャの初期化
    pCollisionManager_ = CollisionManager::GetInstance();
    pCollisionManager_->Initialize();

    /// モデルローダー、ストレージ、マネージャの初期化
    pModelLoader_ = Helper::Model::CreateLoader<ModelLoaderAssimp>(pDirectX_.get());
    pModelStorage_ = Helper::Model::CreateStorage();
    pModelManager_ = Helper::Model::CreateManager(pModelLoader_.get(), pModelStorage_.get());

    // モデルマネージャをシーンマネージャにセット
    pSceneManager_->SetModelManager(pModelManager_.get());

    /// Dissolve用のテクスチャをロード
    pTextureManager_->LoadTexture("noise0.png");
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

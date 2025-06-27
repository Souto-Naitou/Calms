#include "WinterGame.h"

#include <Common/define.h>
#include <Features/SceneManager/SceneManager.h>
#include <Scene/Factory/SceneFactory.h>

#pragma comment(lib, "Ws2_32.lib")

void WinterGame::Initialize()
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

    /// モデルを全てロード
    pModelManager_->LoadAllModel();

    // Dissolve用のテクスチャをロード
    pTextureManager_->LoadTexture("noise0.png");
    pPEDissolve_->SetTextureResource(pTextureManager_->GetTextureResource("noise0.png"));
}

void WinterGame::Finalize()
{
    /// 基底クラスの終了処理
    NimaFramework::Finalize();
}

void WinterGame::Update()
{
    /// 当たり判定の更新
    pCollisionManager_->CheckAllCollision();

    /// 基底クラスの更新処理
    NimaFramework::Update();

}

void WinterGame::Draw()
{
    /// 描画前処理
    NimaFramework::PreProcess();


    /// バックバッファ書き込み
    NimaFramework::Draw();


    /// 描画後処理
    NimaFramework::PostProcess();
}

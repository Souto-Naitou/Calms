#include "WinterGame.h"

#include <Common/define.h>
#include <Features/SceneManager/SceneManager.h>
#include <Scene/Factory/SceneFactory.h>

#pragma comment(lib, "Ws2_32.lib")

void WinterGame::Initialize()
{
    /// 基底クラスの初期化処理
    NimaFramework::Initialize();

    /// シーンファクトリの設定
    pSceneFactory_ = new SceneFactory();
    pSceneManager_->SetSceneFactory(pSceneFactory_);

    /// 当たり判定マネージャの初期化
    pCollisionManager_ = CollisionManager::GetInstance();
    pCollisionManager_->Initialize();

    /// 自動ロードパスの追加
    pModelManager_->AddAutoLoadPath("resources/models");

    /// モデルを全てロード
    pModelManager_->LoadAllModel();

    /// シーンの生成
    pSceneManager_->ReserveScene("TitleScene");

    /// テクスチャの検索パスを追加
    pTextureManager_->AddSearchPath("resources/images");

    /// イージングマネージャの初期化
    pEasingManager_ = EasingManager::GetInstance();

    /// CSVLoaderの初期化
    pCSVLoader_ = CSVLoader::GetInstance();
}

void WinterGame::Finalize()
{
    /// 基底クラスの終了処理
    NimaFramework::Finalize();

    pSceneManager_->Finalize();

    /// シーンファクトリの解放
    delete pSceneFactory_;
}

void WinterGame::Update()
{
    /// 基底クラスの更新処理
    NimaFramework::Update();

    /// シーン更新
    pSceneManager_->Update();

    /// 当たり判定の更新
    pCollisionManager_->CheckAllCollision();

    /// パーティクル更新
    pParticleManager_->Update();
}

void WinterGame::Draw()
{
    /// 描画処理
    pDirectX_->PresentDraw();
    pSRVManager_->PresentDraw();

    /// 背景スプライトの描画
    pSpriteSystem_->PresentDraw();
    pSceneManager_->SceneDraw2dBackGround();

    /// 3D描画
    pObject3dSystem_->PresentDraw();
    pSceneManager_->SceneDraw3d();

    /// 中景スプライトの描画
    pSpriteSystem_->PresentDraw();
    pSceneManager_->SceneDraw2dMidground();

    /// 中景3dオブジェクトの描画
    pObject3dSystem_->PresentDraw();
    pSceneManager_->SceneDraw3dMidground();

    /// ライン描画
    pLineSystem_->PresentDraw();
    pSceneManager_->SceneDrawLine();

    /// パーティクル描画
    pParticleSystem_->PresentDraw();
    pParticleManager_->Draw();

    /// 前景スプライトの描画
    pSpriteSystem_->PresentDraw();
    pSceneManager_->SceneDraw2dForeground();

    pImGuiManager_->EndFrame();
    pDirectX_->PostDraw();
}

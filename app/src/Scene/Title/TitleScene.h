#pragma once

#include <Scene/SceneBase.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Input/Input.h>
#include <Effects/SceneTransition/SceneTransitionManager.h>
#include <Features/Sprite/Sprite.h>
#include <memory>
#include <Features/Cubemap/Skybox.h>
#include <Features/Cubemap/CubemapSystem.h>
#include <Core/Win32/WinSystem.h>
#include <Effects/PostEffects/RandomFilter/RandomFilter.h>
#include <Core/DirectX12/DirectX12.h>
#include <Core/DirectX12/PostEffectExecuter.h>
#include "Animation/OpeningAnimation.h"

/// <summary>
/// タイトルシーン
/// </summary>
class TitleScene : public SceneBase
{
public:
    TitleScene(ISceneArgs* _pArg) : SceneBase(_pArg) {};

    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 終了
    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 描画(テキスト描画を除く)
    /// </summary>
    void Draw() override;

    /// <summary>
    /// テキスト描画
    /// </summary>
    void DrawTexts() override;


private:
    void InitializeGameEye();
    void InitializeSprites();
    void InitializeSkybox();

    void UpdateTitleAnimation();
    void UpdateStartPromptAnimation();

    std::unique_ptr<GameEye>            gameEye_                = {};           // !< ゲームアイ
    std::unique_ptr<Sprite>             pSpriteTitle_           = nullptr;      // !< タイトル
    std::unique_ptr<Sprite>             pSpriteFrameScreen_     = nullptr;      // !< タイトル
    std::unique_ptr<Sprite>             pSpritePressStart_      = nullptr;      // !< メニュー
    std::unique_ptr<Skybox>             pSkybox_                = nullptr;      // !< スカイボックス
    std::unique_ptr<OpeningAnimation>   pOpeningAnimation_      = nullptr;      // !< オープニングアニメーション
    float                               opacityStartPrompt_     = 0.0f;         // !< スタートプロンプトの不透明度
    const float                         kPosYTitle_             = WinSystem::clientHeight / 2.0f - 50.0f ; // !< タイトルのY座標
    std::unique_ptr<RandomFilter>       pRandomFilter_          = nullptr;      // !< ランダムフィルタ
    DirectX12*                          pDx12_                  = nullptr;      // !< DirectX12
    PostEffectExecuter*                 pPostEffectExecuter_    = nullptr;      // !< ポストエフェクト実行クラス


private:
    Input*                      pInput_             = nullptr;      // !< 入力
    SceneTransitionManager*     pSceneTransition_   = nullptr;      // !< シーン遷移
    CubemapSystem*              pCubemapSystem_     = nullptr;      // !< キューブマップシステム
};
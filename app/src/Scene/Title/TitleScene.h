#pragma once

#include <Scene/SceneBase.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Input/Input.h>
#include <Features/SceneManager/SceneManager.h>
#include <drawable/sprite/Sprite.h>
#include <memory>
#include <Features/Cubemap/Skybox.h>
#include <Features/Cubemap/CubemapSystem.h>
#include <Core/Win32/WinSystem.h>
#include <Effects/PostEffects/RandomFilter/RandomFilter.h>
#include <Core/DirectX12/DirectX12.h>
#include <Core/DirectX12/PostEffectExecutor.h>
#include "./Animation/OpeningAnimation.h"
#include <Features/Layer/Canvas.h>
#include <Effects/PostEffects/GaussianBloom/GaussianBloom.h>
#include <Effects/PostEffects/RadialBlur/RadialBlur.h>
#include <Effects/SceneTransition/TransShutter.h>
#include "nima_engine/src/Features/Audio/Audio.h"

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
    /// 描画
    /// </summary>
    void Draw() override;


private:
    void InitializeGameEye();
    void InitializeSprites();
    void InitializeSkybox();
    void InitializePostEffects();

    /// <summary>
    /// タイトルロゴのアニメーション更新を行います。
    /// </summary>
    void UpdateTitleAnimation();

    /// <summary>
    /// 「Press Start」等の開始プロンプトのアニメーション更新を行います。
    /// </summary>
    void UpdateStartPromptAnimation();

    bool                                isChangingScene_    = false;
    std::unique_ptr<TransShutter>       pTransShutter_      = nullptr;      // !< シャッター遷移エフェクト
    std::unique_ptr<Canvas>             pCanvasBack_        = nullptr;      // !< タイトルキャンバス
    std::unique_ptr<Canvas>             pCanvasSprite_      = nullptr;      // !< タイトルキャンバス
    std::unique_ptr<GameEye>            gameEye_            = {};           // !< ゲームアイ
    std::unique_ptr<Sprite>             pSpriteTitle_       = nullptr;      // !< タイトル
    std::unique_ptr<Sprite>             pSpriteFrameScreen_ = nullptr;      // !< タイトル
    std::unique_ptr<Sprite>             pSpritePressStart_  = nullptr;      // !< メニュー
    std::unique_ptr<Skybox>             pSkybox_            = nullptr;      // !< スカイボックス
    std::unique_ptr<OpeningAnimation>   pOpeningAnimation_  = nullptr;      // !< オープニングアニメーション
    float                               opacityStartPrompt_ = 0.0f;         // !< スタートプロンプトの不透明度
    const float                         kPosYTitle_         = WinSystem::clientHeight / 2.0f - 50.0f ; // !< タイトルのY座標
    RandomFilter*                       pRandomFilter_      = nullptr;      // !< ランダムフィルタ
    GaussianBloom*                      pGaussianBloom_     = nullptr;      // !< ガウスぼかし
    RadialBlur*                         pRadialBlur_        = nullptr;      // !< 放射状ブラー
    Audio*                              pSoundStartButton_  = nullptr;      // !< スタートボタン音声

    /// 他クラスのインスタンス
    PostEffectExecutor*         pPostEffectExecutor_    = nullptr;      // !< ポストエフェクト実行クラス
    DirectX12*                  pDx12_                  = nullptr;      // !< DirectX12
    Input*                      pInput_                 = nullptr;      // !< 入力
    SceneManager*               pSceneManager_          = nullptr;      // !< シーン遷移
    CubemapSystem*              pCubemapSystem_         = nullptr;      // !< キューブマップシステム
};
#pragma once

#include <Scene/SceneBase.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Input/Input.h>
#include <Features/SceneManager/SceneManager.h>
#include <drawable/sprite/Sprite.h>
#include <memory>
#include <Features/Cubemap/Skybox.h>
#include <Features/Cubemap/CubemapSystem.h>
#include <Core/Window/Window.h>
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
/// 繧ｿ繧､繝医Ν繧ｷ繝ｼ繝ｳ
/// </summary>
class TitleScene : public SceneBase
{
public:
    TitleScene(ISceneArgs* _pArg) : SceneBase(_pArg) {};

    /// <summary>
    /// 蛻晄悄蛹・    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 邨ゆｺ・    /// </summary>
    void Finalize() override;

    /// <summary>
    /// 譖ｴ譁ｰ
    /// </summary>
    void Update() override;

    /// <summary>
    /// 謠冗判
    /// </summary>
    void Draw() override;


private:
    void InitializeGameEye();
    void InitializeSprites();
    void InitializeSkybox();
    void InitializePostEffects();

    /// <summary>
    /// 繧ｿ繧､繝医Ν繝ｭ繧ｴ縺ｮ繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ譖ｴ譁ｰ繧定｡後＞縺ｾ縺吶・    /// </summary>
    void UpdateTitleAnimation();

    /// <summary>
    /// 縲訓ress Start縲咲ｭ峨・髢句ｧ九・繝ｭ繝ｳ繝励ヨ縺ｮ繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ譖ｴ譁ｰ繧定｡後＞縺ｾ縺吶・    /// </summary>
    void UpdateStartPromptAnimation();

    bool                                isChangingScene_    = false;
    std::unique_ptr<TransShutter>       pTransShutter_      = nullptr;      // !< 繧ｷ繝｣繝・ち繝ｼ驕ｷ遘ｻ繧ｨ繝輔ぉ繧ｯ繝・    std::unique_ptr<Canvas>             pCanvasBack_        = nullptr;      // !< 繧ｿ繧､繝医Ν繧ｭ繝｣繝ｳ繝舌せ
    std::unique_ptr<Canvas>             pCanvasSprite_      = nullptr;      // !< 繧ｿ繧､繝医Ν繧ｭ繝｣繝ｳ繝舌せ
    std::unique_ptr<GameEye>            gameEye_            = {};           // !< 繧ｲ繝ｼ繝繧｢繧､
    std::unique_ptr<Sprite>             pSpriteTitle_       = nullptr;      // !< 繧ｿ繧､繝医Ν
    std::unique_ptr<Sprite>             pSpriteFrameScreen_ = nullptr;      // !< 繧ｿ繧､繝医Ν
    std::unique_ptr<Sprite>             pSpritePressStart_  = nullptr;      // !< 繝｡繝九Η繝ｼ
    std::unique_ptr<Skybox>             pSkybox_            = nullptr;      // !< 繧ｹ繧ｫ繧､繝懊ャ繧ｯ繧ｹ
    std::unique_ptr<OpeningAnimation>   pOpeningAnimation_  = nullptr;      // !< 繧ｪ繝ｼ繝励ル繝ｳ繧ｰ繧｢繝九Γ繝ｼ繧ｷ繝ｧ繝ｳ
    float                               opacityStartPrompt_ = 0.0f;         // !< 繧ｹ繧ｿ繝ｼ繝医・繝ｭ繝ｳ繝励ヨ縺ｮ荳埼乗・蠎ｦ
    const float                         kPosYTitle_         = Window::clientHeight / 2.0f - 50.0f ; // !< 繧ｿ繧､繝医Ν縺ｮY蠎ｧ讓・    RandomFilter*                       pRandomFilter_      = nullptr;      // !< 繝ｩ繝ｳ繝繝繝輔ぅ繝ｫ繧ｿ
    GaussianBloom*                      pGaussianBloom_     = nullptr;      // !< 繧ｬ繧ｦ繧ｹ縺ｼ縺九＠
    RadialBlur*                         pRadialBlur_        = nullptr;      // !< 謾ｾ蟆・憾繝悶Λ繝ｼ
    Audio*                              pSoundStartButton_  = nullptr;      // !< 繧ｹ繧ｿ繝ｼ繝医・繧ｿ繝ｳ髻ｳ螢ｰ
    Audio*                              pSoundBGM_          = nullptr;      // !< BGM髻ｳ螢ｰ

    /// 莉悶け繝ｩ繧ｹ縺ｮ繧､繝ｳ繧ｹ繧ｿ繝ｳ繧ｹ
    PostEffectExecutor*         pPostEffectExecutor_    = nullptr;      // !< 繝昴せ繝医お繝輔ぉ繧ｯ繝亥ｮ溯｡後け繝ｩ繧ｹ
    DirectX12*                  pDx12_                  = nullptr;      // !< DirectX12
    Input*                      pInput_                 = nullptr;      // !< 蜈･蜉・    SceneManager*               pSceneManager_          = nullptr;      // !< 繧ｷ繝ｼ繝ｳ驕ｷ遘ｻ
    CubemapSystem*              pCubemapSystem_         = nullptr;      // !< 繧ｭ繝･繝ｼ繝悶・繝・・繧ｷ繧ｹ繝・Β
};

#pragma once

#include <Scene/SceneBase.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Input/Input.h>
#include <Features/SceneTransition/SceneTransitionManager.h>
#include <Features/Sprite/Sprite.h>
#include <Features/Particle/Emitter/ParticleEmitter.h>
#include <Features/Text/Text.h>
#include <ScoreSystem/ScoreSystem.h>
#include <memory>
#include <Features/Cubemap/Skybox.h>
#include <Features/Cubemap/CubemapSystem.h>
#include <AnimationKeyframes/OpeningAnimation.h>

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

    std::unique_ptr<GameEye>            gameEye_                = {};           // !< ゲームアイ
    std::unique_ptr<Sprite>             pSpriteTitle_           = nullptr;      // !< タイトル
    std::unique_ptr<Sprite>             pSpriteStart_           = nullptr;      // !< メニュー
    std::unique_ptr<Skybox>             pSkybox_                = nullptr;      // !< スカイボックス
    std::unique_ptr<OpeningAnimation>   pOpeningAnimation_      = nullptr;      // !< オープニングアニメーション

private:
    Input*                      pInput_             = nullptr;      // !< 入力
    SceneTransitionManager*     pSceneTransition_   = nullptr;      // !< シーン遷移
    CubemapSystem*              pCubemapSystem_     = nullptr;      // !< キューブマップシステム
};
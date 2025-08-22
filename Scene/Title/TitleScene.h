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

    void Draw() override {};

    /// <summary>
    /// テキスト描画
    /// </summary>
    void DrawTexts() override;


private:
    std::unique_ptr<GameEye>                    gameEye_            = {};           // !< ゲームアイ
    std::unique_ptr<Text>                       pTextTitle_         = nullptr;
    std::unique_ptr<Text>                       pTextStart_         = nullptr;
    

private:
    Input*                                      pInput_             = nullptr;      // !< 入力
    SceneTransitionManager*                     pSceneTransition_   = nullptr;      // !< シーン遷移
};
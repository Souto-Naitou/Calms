#pragma once

#include <Interfaces/IScene.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Input/Input.h>
#include <Features/SceneTransition/SceneTransitionManager.h>
#include <Features/Sprite/Sprite.h>
#include <memory>

class TitleScene : public IScene
{
public:
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
    /// 背景描画
    /// </summary>
    void Draw2dBackGround() override;

    /// <summary>
    /// 3D描画
    /// </summary>
    void Draw3d() override;

    /// <summary>
    /// 中景描画
    /// </summary>
    void Draw2dMidground() override;

    /// <summary>
    /// 3D中景描画
    /// </summary>
    void Draw3dMidground() override;

    /// <summary>
    /// ライン描画
    /// </summary>
    void DrawLine() override;

    /// <summary>
    /// 前景描画
    /// </summary>
    void Draw2dForeground() override;


private:
    std::unique_ptr<GameEye>                    gameEye_            = {};           // !< ゲームアイ
    Sprite*                                     pSpace_             = nullptr;
    

private:
    Input*                                      pInput_             = nullptr;      // !< 入力
    SceneTransitionManager*                     pSceneTransition_   = nullptr;      // !< シーン遷移
};
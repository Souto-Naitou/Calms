#pragma once

#include <Scene/SceneBase.h>
#include <Features/Object3d/Object3d.h>
#include <Features/GameEye/GameEye.h>
#include <entity/player/Player.h>
#include <Common/structs.h>
#include <DebugTools/DebugManager/DebugManager.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/RandomGenerator/RandomGenerator.h>
#include <Features/Line/Line.h>
#include <memory>
#include <Features/Model/ModelManager.h>
#include <Core/DirectX12/TextureManager.h>
#include <entity/player/GameOverAnimation.h>

/// <summary>
/// ゲームオーバーテストシーン
/// </summary>
class GameOverTestScene : public SceneBase
{
public:
    GameOverTestScene(ISceneArgs* _args) : SceneBase(_args) {};

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
    /// <summary>
    /// プレイヤーのスロー（低速移動）状態に関連する更新処理を行います。
    /// </summary>
    void PlayerSlowUpdate();

    /// <summary>
    /// <summary>
    /// シーン用のデバッグウィンドウを描画します。
    /// </summary>
    void DebugWindow();

    std::unique_ptr<Object3d>               grid_               = {};       // !< グリッド
    std::unique_ptr<GameEye>                gameEye_            = {};       // !< ゲームアイ
    std::unique_ptr<Player>                 player_             = {};       // !< プレイヤー
    std::unique_ptr<Canvas>                 canvas_             = {};       // !< ゲームキャンバス
    EntityCommonParams                      entityCommonParams_ = {};       // !< エンティティ共通パラメータ
    DirectionalLight                        directionalLight_   = {};       // !< ディレクショナルライト
    PointLight                              pointLight_         = {};       // !< ポイントライト
    TimeMeasurer                            timer_              = {};       // !< タイマー
    bool                                    isChangingScene_    = false;    // !< シーン遷移中かどうか
    std::unique_ptr<Line>                   lines_              = nullptr;  // !< エリア用ライン
    float                                   areaWidth_          = 25.0f;    // !< エリアの幅
    TimeMeasurer                            titleTimer_         = {};       // タイトル表示用タイマー
    std::unique_ptr<GameOverAnimation>      gameOverAnimation_  = {};       // ゲームオーバーアニメーション
    std::unique_ptr<Sprite>                 spaceSprite_        = nullptr;  // スペースキー案内スプライト

    /// デバッグ用
    DebugManager*   pDebugManager_  = nullptr;
    std::string     name_           = "GameOverTestScene";
    float           framerate_      = 60.0f;


private:
    DeltaTimeManager*   deltaTimeManager_   = nullptr;
    RandomGenerator*    randomGenerator_    = nullptr;
    ModelManager*       pModelManager_      = nullptr;
    LineSystem*         pLineSystem_        = nullptr;
    TextureManager*     pTextureManager_    = nullptr;
};
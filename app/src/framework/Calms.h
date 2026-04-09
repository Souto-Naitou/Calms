#pragma once

#include <Framework/NimaFramework.h>
#include <Features/Collision/Manager/CollisionManager.h>
#include <memory>
#include <Features/Model/Loader/IModelLoader.h>
#include <Features/Model/ModelStorage.h>
#include <Features/Model/ModelManager.h>
#include <screen/factory/IntermediateScreenFactory.h>
#include <Features/Input/InputMapper.hpp>
#include <logic/input/InputAction.h>


/// <summary>
/// ゲームのフレームワーククラス
/// ゲーム固有の初期化、終了、更新、描画を行う
/// </summary>
class Calms : public NimaFramework
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
    /// 描画
    /// </summary>
    void Draw() override;

    bool IsExitProgram() { return isExitProgram_; }


private: /// ゲーム内オブジェクト
    void InitializeInputMapper();

    std::unique_ptr<IModelLoader> pModelLoader_ = nullptr; // !< モデルローダー
    std::unique_ptr<ModelStorage> pModelStorage_ = nullptr; // !< モデルストレージ
    std::unique_ptr<ModelManager> pModelManager_ = nullptr; // !< モデルマネージャー
    std::unique_ptr<IntermediateScreenFactory> pIntermediateScreenFactory_ = nullptr; // !< 中間スクリーンファクトリー
    std::unique_ptr<InputMapper<InputActionUI>> pInputMapperUI_ = nullptr; // !< UI用の入力マッパー

    CollisionManager* pCollisionManager_ = nullptr;
};
#pragma once
#include <vector>
#include <memory>
#include <entity/EntityBase.h>


/// <summary>
/// 敵のライフタイムを管理するクラス
/// - 生成アルゴリズムはEnemySpawner
/// - 生成はEnemyFactory
/// </summary>
class EnemyRepository
{
public:
    static constexpr size_t         kNumEnemyMax_ = 0x1000; //!< 敵の最大数

    EnemyRepository();

    /// <summary>
    /// 終了処理を行います。
    /// </summary>
    void Finalize();

     /// <summary>
     /// 敵の状態更新を行います。
     /// </summary>
    void Update();

     /// <summary>
     /// 敵の描画処理を行います。
     /// </summary>
    void Draw1F();

     /// <summary>
     /// ImGuiを用いたデバッグUIの描画を行います。
     /// </summary>
    void ImGui();

    /// <summary>
    /// エンティティを敵のコレクションに追加します。
    /// </summary>
    /// <param name="enemy">追加する敵のエンティティ。</param>
    void Push(std::unique_ptr<EntityBase> enemy);

     /// <summary>
     /// 全ての敵を強制的に死亡させます。
     /// </summary>
    void KillAll();

private:
    void RemoveDeadEnemies();

    std::vector<std::unique_ptr<EntityBase>> enemies_; // !< 敵のコンテナ

    /// 機能
    uint32_t                        countEnemyKilled_   = 0; //!< 倒した敵の数
};
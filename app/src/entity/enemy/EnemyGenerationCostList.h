#pragma once

#include <entity/enemy/EnemyType.h>
#include <unordered_map>


/// 敵の生成コストのリストを管理するクラス
/// - JSONなどの外部データから読み込むことを想定
class EnemyGenerationCostList
{
public:
    using EnemyTypeString = std::string; // !< 敵の種類を表す文字列
    using GenerationCost = uint32_t; // !< 敵の生成コストを表す型

    /// <summary>
    /// JSONファイルから敵の生成コストリストを読み込みます。
    /// </summary>
    /// <param name="jsonFilePath"></param>
    void LoadFromJson(const std::string& jsonFilePath);

    /// <summary>
    /// 指定された敵の種類に対応する生成コストを取得します。
    /// </summary>
    /// <param name="type">敵の種類</param>
    /// <returns>生成コスト</returns>
    GenerationCost GetCost(EnemyType type) const;

private:
    std::unordered_map<EnemyType, GenerationCost> enemyCostMap_; // !< 敵の種類とその生成コストのマップ
};
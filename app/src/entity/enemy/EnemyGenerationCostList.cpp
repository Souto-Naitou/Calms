#include "EnemyGenerationCostList.h"
#include <cassert>
#include <Utility/JSONIO/JSONIO.h>

void EnemyGenerationCostList::LoadFromJson(const std::string& jsonFilePath)
{
    const auto& jsonData = JSONIO::GetInstance()->Load(jsonFilePath);
    // root.EnemyGenerationCost内をループして、敵の種類と生成コストをマップに格納
    const auto& costObjectList = jsonData.at("EnemyGenerationCost").items();

    for (const auto& item : costObjectList)
    {
        const std::string& enemyTypeStr = item.key();
        const GenerationCost cost = item.value().get<GenerationCost>();
        // 敵の種類を文字列からEnemyTypeに変換
        const EnemyType enemyType = EnemyTypeFromString(enemyTypeStr);
        // マップに格納
        enemyCostMap_.emplace(enemyType, cost);
    }
}

EnemyGenerationCostList::GenerationCost EnemyGenerationCostList::GetCost(EnemyType type) const
{
    if (enemyCostMap_.empty())
    {
        // コストリストが空の場合はアサーションして0を返す
        assert(false && "Enemy cost list is empty");
        return 0;
    }

    GenerationCost cost = 0;

    auto it = enemyCostMap_.find(type);
    const bool isFound = it != enemyCostMap_.end();

    if (isFound)
    {
        cost = it->second;
    }
    else
    {
        // 敵の種類がコストリストに見つからない場合はアサーションして0を返す
        assert(false && "Enemy type not found in cost list");
        cost = 0;
    }

    return cost;
}

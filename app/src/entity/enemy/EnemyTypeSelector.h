#pragma once

#include <entity/enemy/EnemyType.h>
#include <vector>

/// 敵の種類選択ロジック
/// 使用先例 : ResourceBasedEnemySpawner
class EnemyTypeSelector
{
public:
    using EnemyTypeList = std::vector<EnemyType>;
    EnemyType GetRandom(EnemyTypeList& enables);
};
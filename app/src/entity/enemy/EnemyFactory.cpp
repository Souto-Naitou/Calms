#include "EnemyFactory.h"



EnemyFactory::EnemyFactory()
{
    // creators_ に敵の種類ごとの生成関数を登録する例
    // creators_["Goblin"] = []() { return std::make_unique<GoblinEnemy>(); };
    // creators_["Orc"] = []() { return std::make_unique<OrcEnemy>(); };
}

std::unique_ptr<EntityBase> EnemyFactory::Create(const std::string& enemyType)
{
    auto it = creators_.find(enemyType);
    if (it != creators_.end())
    {
        return it->second();
    }
    return nullptr;
}

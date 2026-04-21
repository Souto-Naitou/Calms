#include "ResourceBasedEnemySpawner.h"



void ResourceBasedEnemySpawner::BeginNewWave(const WaveInfo& info)
{
    currentWaveInfo_ = info;
}

void ResourceBasedEnemySpawner::Update(WaveState& waveState)
{
    const float kSpawnInterval = 3.0f; // 敵を生成する間隔（秒）
    const float kCurrentElapsedTime = waveState.elapsedTime; // 現在のウェーブの経過時間
    const float kCurrentInterval = kCurrentElapsedTime - lastSpawnedTime_; // 最後に敵を生成してからの時間

    /// 敵を生成する間隔に達していたら
    if (kCurrentInterval >= kSpawnInterval)
    {
        const float kCurrentBudget = static_cast<float>(waveState.currentBudget);
        EnemyTypeSelector::EnemyTypeList enables;

        /// 現在の予算で生成可能な敵の種類をリストに追加
        for (auto& type : kEnemyTypeTable)
        {
            if (enemyCostList_.GetCost(type.type) <= kCurrentBudget)
            {
                enables.push_back(type.type);
            }
        }

        // 生成可能な敵の種類からランダムに1つ選択
        EnemyType spawnType = enemySelector_.GetRandom(enables);
        this->SpawnEnemy(spawnType, waveState);
    }
}

void ResourceBasedEnemySpawner::SpawnEnemy(EnemyType type, WaveState& waveState)
{
    /// 生成して追加
    auto enemy = enemyFactory_.Create(type);
    enemy->Initialize(false);
    repository_.Push(std::move(enemy));

    // 敵を生成したので予算を消費
    waveState.currentBudget -= enemyCostList_.GetCost(type);
    lastSpawnedTime_ = waveState.elapsedTime;
}

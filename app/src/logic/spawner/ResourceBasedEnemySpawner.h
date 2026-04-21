#pragma once
#include <logic/wave/WaveInfo.h>
#include <logic/wave/WaveState.h>
#include <entity/enemy/EnemyRepository.h>
#include <entity/enemy/EnemyGenerationCostList.h>
#include <entity/enemy/EnemyTypeSelector.h>
#include <entity/enemy/EnemyFactory.h>

class ResourceBasedEnemySpawner
{
public:
    ResourceBasedEnemySpawner(EnemyRepository& repository) : repository_(repository) {}

    /// <summary>
    /// 新しいウェーブの開始を通知します
    /// </summary>
    /// <param name="info">ウェーブ情報</param>
    void BeginNewWave(const WaveInfo& info);

    /// <summary>
    /// 敵の生成処理を更新します。消費したBadgetの更新も行います
    /// </summary>
    /// <param name="waveState">ウェーブの状態</param>
    void Update(WaveState& waveState);

private:
    void SpawnEnemy(EnemyType type, WaveState& waveState);

    WaveInfo currentWaveInfo_;
    EnemyRepository& repository_; // !< 敵のリポジトリへの参照
    EnemyGenerationCostList enemyCostList_; // !< 敵の生成コストリスト
    EnemyTypeSelector enemySelector_; // !< 敵の種類選択ロジック
    EnemyFactory enemyFactory_; // !< 敵の生成ロジック
    float lastSpawnedTime_ = 0.0f; // !< 最後に敵を生成した時間
};
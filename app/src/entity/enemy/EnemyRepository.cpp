#include "EnemyRepository.h"


EnemyRepository::EnemyRepository()
{
    // 予め敵の最大数分の領域を確保しておくことで、動的な再確保を減らしパフォーマンスを向上させる
    enemies_.reserve(kNumEnemyMax_);
}

void EnemyRepository::Finalize()
{
    this->KillAll();
}

void EnemyRepository::Update()
{
    /// 敵の更新
    for (auto& enemy : enemies_)
    {
        if (enemy->IsAlive())
        {
            enemy->Update();
        }
    }

    /// 死亡した敵の削除
    this->RemoveDeadEnemies();
}

void EnemyRepository::Draw1F()
{
    for (auto& enemy : enemies_)
    {
        if (enemy->IsAlive())
        {
            enemy->Draw1F();
        }
    }
}

void EnemyRepository::ImGui()
{

}

void EnemyRepository::Push(std::unique_ptr<EntityBase> enemy)
{
    if (enemies_.size() < kNumEnemyMax_)
    {
        enemies_.emplace_back(std::move(enemy));
    }
    else
    {
        LOG_WARNING("敵の最大数に達しているため、これ以上敵を追加できません。");
    }
}

void EnemyRepository::KillAll()
{
    for (auto& enemy : enemies_)
    {
        enemy->Finalize();
        ++countEnemyKilled_;
    }
    enemies_.clear();
}

void EnemyRepository::RemoveDeadEnemies()
{
    enemies_.erase(
        std::remove_if(enemies_.begin(), enemies_.end(),
            [&](auto& e)
    {
        bool isDead = !e->IsAlive();
        if (isDead)
        {
            e->Finalize();
            ++countEnemyKilled_;
        }
        return isDead;
    }
        ),
        enemies_.end()
    );
}

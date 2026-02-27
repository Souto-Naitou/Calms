#pragma once
#include <memory>
#include <entity/enemy/EnemyInitParams.h>
#include <entity/enemy/EnemyType.h>
#include <entity/EntityBase.h>
#include <Vector3.h>

class EnemyFactory
{
public:
    EnemyFactory() = default;
    void SetContext(const EnemyContext& ctx) { context_ = ctx; }
    void SetPosition(const Vector3& position) { context_.position = position; }

    std::unique_ptr<EntityBase> Create(const EnemyType enemyType);

private:
    std::unique_ptr<EntityBase> CreateNormal() const;
    std::unique_ptr<EntityBase> CreateRusher() const;

    EnemyContext context_ = {};
};
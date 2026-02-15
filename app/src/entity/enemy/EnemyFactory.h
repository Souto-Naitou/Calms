#pragma once
#include <memory>
#include <entity/EntityBase.h>
#include <unordered_map>

class EnemyFactory
{
public:
    EnemyFactory();
    std::unique_ptr<EntityBase> Create(const std::string& enemyType);

private:
    std::unique_ptr<EntityBase> CreateNormal();

    std::unordered_map<std::string, std::function<std::unique_ptr<EntityBase>()>> creators_;
};
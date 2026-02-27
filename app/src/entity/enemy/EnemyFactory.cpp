#include "EnemyFactory.h"
#include <Entity/enemy/EnemyNormal.h>
#include <entity/enemy/rusher/EnemyRusher.h>


std::unique_ptr<EntityBase> EnemyFactory::Create(const EnemyType enemyType)
{
    switch (enemyType)
    {
    case EnemyType::Normal:
        return CreateNormal();
    case EnemyType::Rusher:
        return CreateRusher();
    }
    return nullptr;
}

std::unique_ptr<EntityBase> EnemyFactory::CreateNormal() const
{
    EnemyNormalInitParams param;
    param.pModelSelfBody = context_.pModelSelfBody;
    param.position = context_.position;
    param.pDirLight = context_.pDirLight;
    param.pTargetPosition = context_.pTargetPosition;
    return std::make_unique<EnemyNormal>(param);
}

std::unique_ptr<EntityBase> EnemyFactory::CreateRusher() const
{
    EnemyRusherInitParams param;
    param.pModelSelfBody = context_.pModelSelfBody;
    param.position = context_.position;
    param.pDirLight = context_.pDirLight;
    param.pTargetPosition = context_.pTargetPosition;
    return std::make_unique<EnemyRusher>(param);
}

#pragma once
#include <Common/structs.h>
#include <Features/Model/IModel.h>
#include <Vector3.h>
#include <drawable/particle/Particle.h>

struct EnemyInitParams
{
    DirectionalLight*   pDirLight           = nullptr;
    IModel*             pModelSelfBody      = nullptr;      // 本体モデル
    Vector3             position            = {};           // 初期位置
};

struct EnemyRusherInitParams : public EnemyInitParams
{
    const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
};

struct EnemyNormalInitParams : public EnemyInitParams
{
    const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
    Particle*           pParticleTriangle   = nullptr;      // デスパーティクル
    Particle*           pParticleCircle     = nullptr;      // デスパーティクル
};
#pragma once
#include <Common/structs.h>
#include <Features/Model/IModel.h>
#include <Vector3.h>

struct EnemyContext
{
    DirectionalLight*   pDirLight           = nullptr;
    IModel*             pModelSelfBody      = nullptr;      // 本体モデル
    Vector3             position            = {};           // 初期位置
    const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
};

struct EnemyRusherInitParams
{
    DirectionalLight*   pDirLight           = nullptr;
    IModel*             pModelSelfBody      = nullptr;      // 本体モデル
    Vector3             position            = {};           // 初期位置
    const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
};

struct EnemyNormalInitParams
{
    DirectionalLight*   pDirLight           = nullptr;
    IModel*             pModelSelfBody      = nullptr;      // 本体モデル
    Vector3             position            = {};           // 初期位置
    const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
};
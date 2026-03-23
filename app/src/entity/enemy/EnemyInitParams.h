#pragma once
#include <Features/Model/IModel.h>
#include <Vector3.h>
#include <Features/Lighting/DirectionalLight.h>
#include <drawable/object3d/Object3dInstanced.h>

struct EnemyContext
{
    Object3dInstanced*  pObject3dInstanced  = nullptr;
    DirectionalLight*   pDirLight           = nullptr;
    Vector3             position            = {};           // 初期位置
    const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
};

struct EnemyRusherInitParams
{
    DirectionalLight*   pDirLight           = nullptr;
    Object3dInstanced*  pObject3dInstanced  = nullptr;
    Vector3             position            = {};           // 初期位置
    const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
};

struct EnemyNormalInitParams
{
    DirectionalLight*   pDirLight           = nullptr;
    Object3dInstanced*  pObject3dInstanced  = nullptr;
    Vector3             position            = {};           // 初期位置
    const Vector3*      pTargetPosition     = nullptr;      // 追尾対象位置
};
#pragma once

#include <Vector3.h>
#include <Matrix4x4.h>
#include <Features/GameEye/GameEye.h>
#include <Features/Object3d/Object3d.h>

/// <summary>
/// スクリーン座標をワールド座標に変換してレティクルを表示するクラス
/// </summary>
class ScreenToWorld
{
public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw();

    void SetGameEye(GameEye* _eye) { pGameEye_ = _eye; ModifyGameEye(_eye); }
    Vector3 GetWorldPoint() const { return worldPoint_; }

private:
    /// ワールド上の座標
    Vector3 worldPoint_ = {};

    /// Plane Infomation
    Vector3 normal_ = {};

    /// Ray Infomation
    Vector3 origin_ = {};
    Vector3 direction_ = {};

    /// カーソル
    Object3d* object_ = {};

private:
    /// カメラ
    GameEye* pGameEye_ = nullptr;

private:
    void ModifyGameEye(GameEye* _eye);
};
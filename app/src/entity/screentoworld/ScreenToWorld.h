#pragma once

#include <Vector3.h>
#include <Matrix4x4.h>
#include <Features/GameEye/GameEye.h>
#include <drawable/sprite/Sprite.h>
#include <memory>

/// <summary>
/// スクリーン座標をワールド座標に変換してレティクルを表示するクラス
/// </summary>
class ScreenToWorld
{
public:
    /// <summary>
    /// 変換および表示に必要なリソースを初期化します。
    /// </summary>
    void Initialize();

    /// <summary>
    /// リソースの後始末を行います。
    /// </summary>
    void Finalize();

    /// <summary>
    /// マウス位置などからレイを計算し、ワールド座標を更新します。
    /// </summary>
    void Update();

    /// <summary>
    /// レティクル等の描画を行います。
    /// </summary>
    void Draw1F();

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
    std::unique_ptr<Sprite> reticle_ = {};

private:
    /// カメラ
    GameEye* pGameEye_ = nullptr;

private:
    /// <summary>
    /// カメラ変更に伴う内部データの更新を行います。
    /// </summary>
    /// <param name="_eye">新しい GameEye。</param>
    inline void ModifyGameEye(GameEye* _eye) { pGameEye_ = _eye; }
};
#pragma once

#include <drawable/sprite/Sprite.h>
#include <Features/Layer/Canvas.h>
#include <wrapper/InputAwareSprite.h>

/// <summary>
/// 入力ガイドクラス
/// </summary>
class InputGuide
{
public:
    /// <summary>
    /// 入力ガイドの初期化を行います。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 入力ガイドの状態を更新します。
    /// 点滅等の視覚効果を想定しています。
    /// </summary>
    void Update();

    /// <summary>
    /// 入力ガイドの描画を行います。
    /// </summary>
    void Draw1F();

    /// <summary>
    /// 入力ガイドの終了処理を行います。
    /// </summary>
    void Finalize();

private:
    void InitializeInputAwareSprite();

    InputAwareSprite inputAwareSprite_;
    std::unique_ptr<Sprite> shoot_ = nullptr;
    std::unique_ptr<Sprite> slomo_ = nullptr;
    std::unique_ptr<Sprite> explosion_ = nullptr;
    std::unique_ptr<Sprite> pause_ = nullptr;
};
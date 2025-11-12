#pragma once

#include <drawable/sprite/Sprite.h>
#include <Features/Layer/Canvas.h>

/// <summary>
/// 入力ガイドクラス
/// </summary>
class InputGuide
{
public:
    /// <summary>
    /// 入力ガイドの初期化を行います。
    /// </summary>
    void Initialize(Canvas* canvas);

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
    Canvas* canvas_ = nullptr;
    std::unique_ptr<Sprite> leftclick_ = nullptr;
    std::unique_ptr<Sprite> shift_ = nullptr;
};
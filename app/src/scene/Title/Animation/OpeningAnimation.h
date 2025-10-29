#pragma once

#include <Features/Animation/AnimationTimeline.h>
#include <Features/Sprite/Sprite.h>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <Features/Layer/Canvas.h>

/// <summary>
/// オープニングアニメーションクラス
/// </summary>
class OpeningAnimation
{
public:
    OpeningAnimation();
    ~OpeningAnimation() = default;

    /// <summary>
    /// アニメーション用リソースの初期化を行います。
    /// </summary>
    void Initialize(Canvas* canvas);

    /// <summary>
    /// アニメーションの更新を行います。
    /// </summary>
    void Update();

    /// <summary>
    /// 2D 描画を行います。
    /// </summary>
    void Draw2d();

    /// <summary>
    /// 再生を開始します。
    /// </summary>
    void Play();

    /// <summary>
    /// デバッグUIの描画を行います。
    /// </summary>
    void ImGui();

private:
    std::unique_ptr<Sprite> spriteBackground_ = {};
    std::unique_ptr<AnimationTimeline<float>> timelineOpacity_ = {};
    std::unique_ptr<DebugEntry<OpeningAnimation>> pDebugEntry_ = {};
};
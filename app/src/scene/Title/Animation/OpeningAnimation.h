#pragma once

#include <Features/Animation/AnimationTimeline.h>
#include <Features/Sprite/Sprite.h>
#include <DebugTools/DebugEntry/DebugEntry.h>

/// <summary>
/// オープニングアニメーションクラス
/// </summary>
class OpeningAnimation
{
public:
    OpeningAnimation();
    ~OpeningAnimation() = default;

    void Initialize();
    void Update();
    void Draw2d();
    void Play();
    void ImGui();

private:
    std::unique_ptr<Sprite> spriteBackground_ = {};
    std::unique_ptr<AnimationTimeline<float>> timelineOpacity_ = {};
    std::unique_ptr<DebugEntry<OpeningAnimation>> pDebugEntry_ = {};
};
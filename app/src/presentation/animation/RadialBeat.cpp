#include "RadialBeat.h"
#include <Features/Animation/AnimationTween.hpp>
#include <Math/Easing.h>



void RadialBeat::Initialize(RadialBlur* pRadialBlur)
{
    debugEntry_ = std::make_unique<DebugEntry<RadialBeat>>("RadialBeat", "RadialBeat", this, false);

    pRadialBlur_ = pRadialBlur;
    pRadialBlur->SetSamples(4);
    pRadialBlur->SetCenter(0.5f);
    AnimationTween<float> tween0(1.0f, 1.0f, 1.0f, 1.0f);
}

void RadialBeat::Finalize()
{
    pRadialBlur_->SetBlurWidth(0.0f);
}

void RadialBeat::Start(float duration)
{
    AnimationTween<float> tween0(0.0f, duration * 0.1f, 0.0f, 1.0f);
    tween0.SetTransitionFunction(&Math::Easing::EaseInQuad);
    AnimationTween<float> tween1(duration * 0.1f, duration * 0.9f, 1.0f, 0.0f);
    tween1.SetTransitionFunction(&Math::Easing::EaseOutQuad);
    timeline_.ClearTween();
    timeline_.AddTween(tween0);
    timeline_.AddTween(tween1);
    timeline_.Start();
    isPlaying_ = true;
}

void RadialBeat::Update()
{
    if (!isPlaying_) return;

    const float value = timeline_.Update();
    pRadialBlur_->SetBlurWidth(value * maxWidth_);
}

void RadialBeat::ImGui()
{
    timeline_.ImGui("RadialBeat Timeline");
}

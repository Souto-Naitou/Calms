#pragma once
#include <Effects/PostEffects/RadialBlur/RadialBlur.h>
#include <Features/Animation/AnimationTimeline.hpp>
#include <memory>
#include "nima_engine/src/DebugTools/DebugEntry/DebugEntry.h"

class RadialBeat
{
public:
    void Initialize(RadialBlur* pRadialBlur);
    void Finalize();
    void Start(float duration);
    void SetMaxWidth(float width) { maxWidth_ = width; }
    void Update();
    void ImGui();

private:
    std::unique_ptr<DebugEntry<RadialBeat>> debugEntry_ = nullptr;
    RadialBlur* pRadialBlur_ = nullptr;
    AnimationTimeline<float> timeline_;
    float maxWidth_ = 0.0f;
    bool isPlaying_ = false;
};
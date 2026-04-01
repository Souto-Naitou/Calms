#pragma once
#include <core/ISceneLayer.h>
#include <Features/Input/Input.h>

class PauseLayer : public ISceneLayer
{
public:


    void Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer) override;


    void Finalize() override;


    void Update() override;


    void Draw() override;


    void SetPaused(bool paused) { isPaused_ = paused; }
    bool IsPaused() const { return isPaused_; }

private:
    bool isPaused_ = false;
    static constexpr float kButtonSpacingVh_ = 4.0f; // vh単位
    static constexpr float kMarginCenterOffsetVh_ = 20.0f; // vh単位
};
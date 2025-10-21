#pragma once

#include <Features/Sprite/Sprite.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <array>
#include <memory>
#include <Features/Animation/AnimationTimeline.h>


/// <summary>
/// ゲーム開始前のカウントダウンを再生するクラス
/// </summary>
class CountDown
{
public:
    void Initialize();
    void Finalize();
    void Update();
    void Draw2D();

    void Start();
    bool IsStart() const { return !isInit_; }
    bool IsEnd() const { return isEnd_; }

private:
    enum class State
    {
        CountDown,
        Start,
        End
    } currentState_;

    void UpdateCountDown();
    void UpdateStart();

    TimeMeasurer timer_ = {};
    TimeMeasurer startTimer_ = {};

    std::array<std::unique_ptr<Sprite>, 3> numbers_;
    std::unique_ptr<Sprite> start_ = nullptr;

    AnimationTimeline<float> animOpacity_ = {};
    AnimationTimeline<float> animSize_ = {};
    AnimationTimeline<float> animStart_ = {};

    int currentNumber_ = 2;

    bool isDrawStart_ = false;
    bool isInit_ = false;
    bool isEnd_ = false;

    float opacity_ = 1.0f;

    constexpr static float changeInterval_             = 1.0f;
    constexpr static float factorIntervalNumberSpawn_  = 0.1f;
    constexpr static float startDuration_              = 2.0f;
    constexpr static float maxSizeFactorNumber_        = 2.0f;
};
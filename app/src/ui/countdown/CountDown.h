#pragma once

#include <drawable/sprite/Sprite.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <array>
#include <memory>
#include <Features/Animation/AnimationTimeline.hpp>


/// <summary>
/// 繧ｲ繝ｼ繝髢句ｧ句燕縺ｮ繧ｫ繧ｦ繝ｳ繝医ム繧ｦ繝ｳ繧貞・逕溘☆繧九け繝ｩ繧ｹ
/// </summary>
class CountDown
{
public:
    enum class State
    {
        CountDown,
        Start,
        End
    };

    /// <summary>
    /// 繧ｫ繧ｦ繝ｳ繝医ム繧ｦ繝ｳ逕ｨ繝ｪ繧ｽ繝ｼ繧ｹ縺ｮ蛻晄悄蛹悶ｒ陦後＞縺ｾ縺吶・    /// </summary>
    void Initialize();

    /// <summary>
    /// 蠕悟ｧ区忰繧定｡後＞縺ｾ縺吶・    /// </summary>
    void Finalize();

    /// <summary>
    /// 繧ｫ繧ｦ繝ｳ繝医ム繧ｦ繝ｳ縺ｮ迥ｶ諷区峩譁ｰ繧定｡後＞縺ｾ縺吶・    /// </summary>
    void Update();

    /// <summary>
    /// 2D 繧ｹ繝励Λ繧､繝医・謠冗判繧定｡後＞縺ｾ縺吶・    /// </summary>
    void Draw1F();

    /// <summary>
    /// 繧ｫ繧ｦ繝ｳ繝医ム繧ｦ繝ｳ繧帝幕蟋九＠縺ｾ縺吶・    /// </summary>
    void Start();
    bool IsStart() const { return !isInit_; }
    bool IsEnd() const { return isEnd_; }
    State GetState() const { return currentState_; }

private:


    /// <summary>
    /// 謨ｰ蟄励き繧ｦ繝ｳ繝医ム繧ｦ繝ｳ縺ｮ譖ｴ譁ｰ繧定｡後＞縺ｾ縺吶・    /// </summary>
    void UpdateCountDown();

    /// <summary>
    /// START 陦ｨ遉ｺ繝輔ぉ繝ｼ繧ｺ縺ｮ譖ｴ譁ｰ繧定｡後＞縺ｾ縺吶・    /// </summary>
    void UpdateStart();

    TimeMeasurer timer_ = {};
    TimeMeasurer startTimer_ = {};
    std::array<std::unique_ptr<Sprite>, 3> numbers_;
    std::unique_ptr<Sprite> start_ = nullptr;

    AnimationTimeline<float> animOpacity_ = {};
    AnimationTimeline<float> animSize_ = {};
    AnimationTimeline<float> animStart_ = {};

    State currentState_ = State::CountDown;
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

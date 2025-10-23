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
    /// <summary>
    /// カウントダウン用リソースの初期化を行います。
    /// </summary>
    void Initialize();

    /// <summary>
    /// 後始末を行います。
    /// </summary>
    void Finalize();

    /// <summary>
    /// カウントダウンの状態更新を行います。
    /// </summary>
    void Update();

    /// <summary>
    /// 2D スプライトの描画を行います。
    /// </summary>
    void Draw2D();

    /// <summary>
    /// カウントダウンを開始します。
    /// </summary>
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

    /// <summary>
    /// 数字カウントダウンの更新を行います。
    /// </summary>
    void UpdateCountDown();

    /// <summary>
    /// START 表示フェーズの更新を行います。
    /// </summary>
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
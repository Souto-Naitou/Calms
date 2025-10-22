#pragma once

#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Sprite/Sprite.h>
#include <array>

/// <summary>
/// ゲーム内タイマークラス
/// </summary>
class InGameTimer
{
public:
    void Initialize(bool _useSystemClock, double _gameDuration);
    void Update();
    void Draw();
    void Finalize();

    void Start();
    void Reset();

public: /// Getter
    bool IsEnd() const { return isEnd_; }
    bool IsNextScene() const { return isNextScene_; }
    double GetNowTime() const { return nowTime_; }


public: /// Setter
    void SetDisplay(bool _isDisplay) { isDisplay_ = _isDisplay; }
    void SetNowTime(double _time) { nowTime_ = _time; }
    bool SetIsEnd(bool _isEnd) { isEnd_ = _isEnd; }

private:
    void CurrentTimeUpdate();
    void SpriteUpdate();

private:
    /// タイマー
    std::unique_ptr<TimeMeasurer> pTimer_ = nullptr;
    double nowTime_ = 0.0;

    bool isStart_ = false;
    bool isDisplay_ = false;
    bool isUseSystemClock_ = false;

    double gameDuration_ = 0.0;
    const double changeInterval_ = 2.0;

    bool isEnd_ = false;
    bool isNextScene_ = false;

    std::array<std::unique_ptr<Sprite>, 10> tensPlaceNums_ = {};
    std::array<std::unique_ptr<Sprite>, 10> onesPlaceNums_ = {};

    // 表示するスプライトのインデックス
    uint32_t indexTensPlace_ = 0;
    uint32_t indexOnesPlace_ = 0;
};
#pragma once

#include <Timer/Timer.h>
#include <Features/Sprite/Sprite.h>
#include <array>

class GameTimer
{
public:
    void Initialize(bool _useSystemClock);
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
    /// タイマー
    Timer* pTimer_ = nullptr;
    double nowTime_ = 0.0;

    bool isStart_ = false;
    bool isDisplay_ = false;
    bool isUseSystemClock_ = false;

    const double gameDuration_ = 60.0;
    const double changeInterval_ = 2.0;

    bool isEnd_ = false;
    bool isNextScene_ = false;

    std::array<Sprite*, 10> tensPlaceNums_;
    std::array<Sprite*, 10> onesPlaceNums_;
};
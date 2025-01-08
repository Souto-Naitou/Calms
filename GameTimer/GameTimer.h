#pragma once

#include <Timer/Timer.h>
#include <Features/Sprite/Sprite.h>
#include <array>

class GameTimer
{
public:
    void Initialize();
    void Update();
    void Draw();
    void Finalize();

    void Start();
    void Reset();

    bool IsEnd() const { return isEnd_; }
    bool IsNextScene() const { return isNextScene_; }

private:
    /// タイマー
    double nowTime_ = 0.0;
    bool isStart_ = false;

    const double gameDuration_ = 60.0;
    const double changeInterval_ = 2.0;

    bool isEnd_ = false;
    bool isNextScene_ = false;

    std::array<Sprite*, 10> tensPlaceNums_;
    std::array<Sprite*, 10> onesPlaceNums_;
};
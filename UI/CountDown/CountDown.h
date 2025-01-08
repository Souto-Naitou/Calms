#pragma once

#include <Features/Sprite/Sprite.h>
#include <Timer/Timer.h>
#include <array>

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
    Timer timer_ = {};
    Timer startTimer_ = {};

    std::array<Sprite*, 3> numbers_;
    Sprite* start_ = nullptr;

    int currentNumber_ = 2;

    bool isDrawStart_ = false;
    bool isInit_ = false;
    bool isEnd_ = false;

    float opacity_ = 1.0f;

    double changeInterval_ = 1.0;
    double startDuration_ = 2.0;
};
#pragma once

#include <Features/Sprite/Sprite.h>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <array>
#include <memory>

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
    TimeMeasurer timer_ = {};
    TimeMeasurer startTimer_ = {};

    std::array<std::unique_ptr<Sprite>, 3> numbers_;
    std::unique_ptr<Sprite> start_ = nullptr;

    int currentNumber_ = 2;

    bool isDrawStart_ = false;
    bool isInit_ = false;
    bool isEnd_ = false;

    float opacity_ = 1.0f;

    double changeInterval_ = 1.0;
    double startDuration_ = 2.0;
};
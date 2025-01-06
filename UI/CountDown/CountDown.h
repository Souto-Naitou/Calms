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
    bool IsEnd() const { return isEnd_; }

private:
    Timer timer_ = {};

    std::array<Sprite*, 3> numbers_;
    int currentNumber_ = 2;

    bool isEnd_ = true;

    float opacity_ = 1.0f;
    double changeInterval_ = 1.0;
};
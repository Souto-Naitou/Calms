#pragma once
#include <Windows.h>

class Timer
{
public:
    Timer();
    ~Timer() {};

    void Start();
    void Reset();
    double GetNow();

    bool GetIsStart() const { return isStart_; }

private:
    LARGE_INTEGER mFreq_    = {};
    LARGE_INTEGER mStart_   = {};
    double now_             = 0.0;
    bool isStart_           = false;
};
#include "Timer.h"

Timer::Timer()
{
    QueryPerformanceFrequency(&mFreq_);
}

void Timer::Start()
{
    isStart_ = true;
    QueryPerformanceCounter(&mStart_);
}

void Timer::Reset()
{
    mStart_ = {};
    now_ = 0.0;
    isStart_ = false;
}

double Timer::GetNow()
{
    LARGE_INTEGER mNow = {};
    QueryPerformanceCounter(&mNow);

    now_ = static_cast<double>(mNow.QuadPart - mStart_.QuadPart) / static_cast<double>(mFreq_.QuadPart);

    return now_;
}

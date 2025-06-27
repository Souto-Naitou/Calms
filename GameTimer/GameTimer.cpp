#include "GameTimer.h"
#include <Core/Win32/WinSystem.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>

void GameTimer::Reset()
{
    isEnd_ = false;
    isNextScene_ = false;
    isStart_ = false;
    nowTime_ = 0.0;
    if (pTimer_) pTimer_->Reset();
}

void GameTimer::Start()
{
    isStart_ = true;

    if (isUseSystemClock_)
    {
        pTimer_->Start();
    }
}

void GameTimer::Initialize(bool _useSystemClock, double _gameDuration)
{
    gameDuration_ = _gameDuration;

    for (int i = 0; i < 10; i++)
    {
        tensPlaceNums_[i] = new Sprite();
        tensPlaceNums_[i]->SetName("tensPlaceNum_" + std::to_string(i));
        tensPlaceNums_[i]->Initialize("number_" + std::to_string(i) + ".png");
        tensPlaceNums_[i]->SetAnchorPoint({ 0.5f, 0.5f });
        tensPlaceNums_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 0.2f });
        tensPlaceNums_[i]->SetSizeMultiply(0.75f);

        onesPlaceNums_[i] = new Sprite();
        onesPlaceNums_[i]->SetName("onesPlaceNum_" + std::to_string(i));
        onesPlaceNums_[i]->Initialize("number_" + std::to_string(i) + ".png");
        onesPlaceNums_[i]->SetAnchorPoint({ 0.5f, 0.5f });
        onesPlaceNums_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 0.2f });
        onesPlaceNums_[i]->SetSizeMultiply(0.75f);
    }

    if (_useSystemClock)
    {
        pTimer_ = new Timer();
    }

    isUseSystemClock_ = _useSystemClock;
}

void GameTimer::Update()
{
    /// タイマー更新
    if (isStart_)
    {
        if (isUseSystemClock_)
        {
            nowTime_ = pTimer_->GetNow<float>();
        }
        else
        {
            nowTime_ += DeltaTimeManager::GetInstance()->GetDeltaTime(1);
        }
    }

    if (nowTime_ >= gameDuration_)
    {
        isEnd_ = true;
    }
    if (nowTime_ > gameDuration_ + changeInterval_)
    {
        isNextScene_ = true;
    }

    for (int i = 0; i < 10; i++)
    {
        tensPlaceNums_[i]->Update();
        onesPlaceNums_[i]->Update();
    }
}

void GameTimer::Draw()
{
    if (!isDisplay_)
    {
        return;
    }

    double time = gameDuration_ - nowTime_;
    int tens_place = static_cast<int>(time) / 10;
    int ones_place = static_cast<int>(time) % 10;

    if (tens_place < 0) tens_place = 0;
    if (ones_place < 0) ones_place = 0;

    tensPlaceNums_[tens_place]->SetPosition({
        WinSystem::clientWidth / 2 - tensPlaceNums_[tens_place]->GetSize().x / 3.0f,
        static_cast<float>(WinSystem::clientHeight / 4)
        }
    );

    tensPlaceNums_[tens_place]->Update();
    tensPlaceNums_[tens_place]->Draw();


    onesPlaceNums_[ones_place]->SetPosition({
        WinSystem::clientWidth / 2 + onesPlaceNums_[ones_place]->GetSize().x / 3.0f,
        static_cast<float>(WinSystem::clientHeight / 4)
        }
    );

    onesPlaceNums_[ones_place]->Update();
    onesPlaceNums_[ones_place]->Draw();
}

void GameTimer::Finalize()
{
    for (int i = 0; i < 10; i++)
    {
        tensPlaceNums_[i]->Finalize();
        delete tensPlaceNums_[i];
        onesPlaceNums_[i]->Finalize();
        delete onesPlaceNums_[i];
    }
}

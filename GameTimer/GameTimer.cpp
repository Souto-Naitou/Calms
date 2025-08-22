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

void GameTimer::CurrentTimeUpdate()
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
}

void GameTimer::SpriteUpdate()
{
    double time = gameDuration_ - nowTime_;
    indexTensPlace_ = static_cast<int>(time) / 10;
    indexOnesPlace_ = static_cast<int>(time) % 10;

    // インデックスの範囲内に収める
    indexTensPlace_ = std::clamp(indexTensPlace_, 0u, 9u);
    indexOnesPlace_ = std::clamp(indexOnesPlace_, 0u, 9u);

    auto& spriteTensPlace = tensPlaceNums_[indexTensPlace_];
    auto& spriteOnesPlace = onesPlaceNums_[indexOnesPlace_];

    // Y座標（固定）
    const float kSpritePositionY = static_cast<float>(WinSystem::clientHeight / 4);

    spriteTensPlace->SetPosition({
        WinSystem::clientWidth / 2 - spriteTensPlace->GetSize().x / 3.0f,
        kSpritePositionY
        }
    );

    spriteOnesPlace->SetPosition({
        WinSystem::clientWidth / 2 + spriteOnesPlace->GetSize().x / 3.0f,
        kSpritePositionY
        }
    );

    spriteTensPlace->Update();
    spriteOnesPlace->Update();
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
        tensPlaceNums_[i] = std::make_unique<Sprite>();
        tensPlaceNums_[i]->SetName("tensPlaceNum_" + std::to_string(i));
        tensPlaceNums_[i]->Initialize("number_" + std::to_string(i) + ".png");
        tensPlaceNums_[i]->SetAnchorPoint({ 0.5f, 0.5f });
        tensPlaceNums_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 0.2f });
        tensPlaceNums_[i]->SetSizeMultiply(0.75f);

        onesPlaceNums_[i] = std::make_unique<Sprite>();
        onesPlaceNums_[i]->SetName("onesPlaceNum_" + std::to_string(i));
        onesPlaceNums_[i]->Initialize("number_" + std::to_string(i) + ".png");
        onesPlaceNums_[i]->SetAnchorPoint({ 0.5f, 0.5f });
        onesPlaceNums_[i]->SetColor({ 1.0f, 1.0f, 1.0f, 0.2f });
        onesPlaceNums_[i]->SetSizeMultiply(0.75f);
    }

    if (_useSystemClock)
    {
        pTimer_ = std::make_unique<TimeMeasurer>();
    }

    isUseSystemClock_ = _useSystemClock;
}

void GameTimer::Update()
{
    this->CurrentTimeUpdate();

    this->SpriteUpdate();

    //for (int i = 0; i < 10; i++)
    //{
    //    tensPlaceNums_[i]->Update();
    //    onesPlaceNums_[i]->Update();
    //}
}

void GameTimer::Draw()
{
    if (!isDisplay_)
    {
        return;
    }

    tensPlaceNums_[indexTensPlace_]->Draw();
    onesPlaceNums_[indexOnesPlace_]->Draw();
}

void GameTimer::Finalize()
{
    for (int i = 0; i < 10; i++)
    {
        tensPlaceNums_[i]->Finalize();
        onesPlaceNums_[i]->Finalize();
    }
}

#include "CountDown.h"

#include <MathExtension/mathExtension.h>
#include <Core/Win32/WinSystem.h>

void CountDown::Initialize()
{
    /// 数字 1 - 3 (256 x 256)
    for (int i = 0; i < 3; i++)
    {
        numbers_[i] = new Sprite();
    }

    Vector2 clientSize = { static_cast<float>(WinSystem::kClientWidth), static_cast<float>(WinSystem::kClientHeight) };

    numbers_[0]->Initialize("number_1.png");
    numbers_[0]->SetName("countDown_1");
    numbers_[0]->SetAnchorPoint({0.5f, 0.5f});
    numbers_[0]->SetPosition(clientSize / 2.0f);
    
    numbers_[1]->Initialize("number_2.png");
    numbers_[1]->SetName("countDown_2");
    numbers_[1]->SetAnchorPoint({ 0.5f, 0.5f });
    numbers_[1]->SetPosition(clientSize / 2.0f);
    
    numbers_[2]->Initialize("number_3.png");
    numbers_[2]->SetName("countDown_3");
    numbers_[2]->SetAnchorPoint({ 0.5f, 0.5f });
    numbers_[2]->SetPosition(clientSize / 2.0f);

    start_ = new Sprite();
    start_->Initialize("start.png");
    start_->SetName("start");
    start_->SetAnchorPoint({ 0.5f, 0.5f });
    start_->SetPosition(clientSize / 2.0f);

    timer_.Reset();
    startTimer_.Reset();

    isInit_ = true;
}

void CountDown::Finalize()
{
    for (int i = 0; i < 3; i++)
    {
        numbers_[i]->Finalize();
        delete numbers_[i];
    }

    start_->Finalize();
    delete start_;
}

void CountDown::Update()
{
    if (isEnd_ || isInit_) return;

    if (timer_.GetNow() > changeInterval_ && !isDrawStart_)
    {
        currentNumber_--;
        timer_.Reset();
        timer_.Start();
    }

    if (currentNumber_ < 0)
    {
        isDrawStart_ = true;
        currentNumber_ = 2;
        startTimer_.Reset();
        startTimer_.Start();
    }

    if (startTimer_.GetNow() > startDuration_ && startTimer_.GetIsStart())
    {
        isEnd_ = true;
    }

    /// 透明度を変更
    if (!isDrawStart_)
    {
        opacity_ = Math::Lerp(1.0f, 0.0f, static_cast<float>(timer_.GetNow() / changeInterval_));
        numbers_[currentNumber_]->SetColor({ 1.0f, 1.0f, 1.0f, opacity_ });
        numbers_[currentNumber_]->Update();
    }
    else
    {
        if (startTimer_.GetNow() > startDuration_ - 1.0)
        {
            opacity_ = Math::Lerp(1.0f, 0.0f, static_cast<float>(startTimer_.GetNow() - (startDuration_ - 1.0) / 1.0));
        }
        else
        {
            opacity_ = 1.0f;
        }
        start_->SetColor({ 1.0f, 1.0f, 1.0f, opacity_ });
        start_->Update();
    }
}

void CountDown::Draw2D()
{
    if (isEnd_|| isInit_) return;

    if (!isDrawStart_)      numbers_[currentNumber_]->Draw();
    else                    start_->Draw();
}

void CountDown::Start()
{
    timer_.Reset();
    timer_.Start();
    startTimer_.Reset();

    isDrawStart_ = false;
    isEnd_ = false;
    isInit_ = false;
    currentNumber_ = 2;
}

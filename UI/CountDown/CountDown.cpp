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

    numbers_[0]->Initialize("countDown_1.png");
    numbers_[0]->SetName("countDown_1");
    numbers_[0]->SetAnchorPoint({0.5f, 0.5f});
    numbers_[0]->SetPosition(clientSize / 2.0f);
    
    numbers_[1]->Initialize("countDown_2.png");
    numbers_[1]->SetName("countDown_2");
    numbers_[1]->SetAnchorPoint({ 0.5f, 0.5f });
    numbers_[1]->SetPosition(clientSize / 2.0f);
    
    numbers_[2]->Initialize("countDown_3.png");
    numbers_[2]->SetName("countDown_3");
    numbers_[2]->SetAnchorPoint({ 0.5f, 0.5f });
    numbers_[2]->SetPosition(clientSize / 2.0f);
}

void CountDown::Finalize()
{
    for (int i = 0; i < 3; i++)
    {
        numbers_[i]->Finalize();
        delete numbers_[i];
    }
}

void CountDown::Update()
{
    if (isEnd_) return;

    if (timer_.GetNow() > changeInterval_)
    {
        currentNumber_--;
        timer_.Reset();
        timer_.Start();
    }

    if (currentNumber_ < 0)
    {
        isEnd_ = true;
        currentNumber_ = 2;
    }

    /// 透明度を変更
    opacity_ = Math::Lerp(1.0f, 0.0f, static_cast<float>(timer_.GetNow() / changeInterval_));
    numbers_[currentNumber_]->SetColor({ 1.0f, 1.0f, 1.0f, opacity_ });

    numbers_[currentNumber_]->Update();
}

void CountDown::Draw2D()
{
    if (isEnd_) return;
    numbers_[currentNumber_]->Draw();
}

void CountDown::Start()
{
    timer_.Reset();
    timer_.Start();
    isEnd_ = false;
    currentNumber_ = 2;
}

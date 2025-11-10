#include "CountDown.h"

#include <MathExtension/mathExtension.h>
#include <Core/Win32/WinSystem.h>
#include <config/ResourcePath.h>
#include <string>
#include <Math/Easing.h>

void CountDown::Initialize()
{
    Vector2 clientSize = { static_cast<float>(WinSystem::clientWidth), static_cast<float>(WinSystem::clientHeight) };

    /// 数字 1 - 3 (256 x 256)
    for (int i = 0; i < 3; i++)
    {
        numbers_[i] = std::make_unique<Sprite>();
        numbers_[i]->Initialize(Path::Image::kNumbers[i + 1]);
        numbers_[i]->SetName("countDown_" + std::to_string(i));
        numbers_[i]->SetAnchorPoint({ 0.5f, 0.5f });
        numbers_[i]->SetPosition(clientSize / 2.0f);
    }

    start_ = std::make_unique<Sprite>();
    start_->Initialize(Path::Image::kCountDownEndString);
    start_->SetName("start");
    start_->SetAnchorPoint({ 0.5f, 0.5f });
    start_->SetPosition(clientSize / 2.0f);

    timer_.Reset();
    startTimer_.Reset();

    AnimationTween<float> tween(0.0f, changeInterval_, 1.0f, 0.0f);
    AnimationTween<float> sizeTween1(0.0f, changeInterval_ * factorIntervalNumberSpawn_, 0.0f, 1.0f);
    AnimationTween<float> sizeTween2(0.0f, changeInterval_ * (1.0f - factorIntervalNumberSpawn_), 1.0f, 0.8f);
    sizeTween1.SetTransitionFunction(Math::Easing::EaseOutSine);
    sizeTween2.SetTransitionFunction(Math::Easing::EaseInOutCubic);
    animOpacity_.AddTween(tween);
    animSize_.AddTween(sizeTween1);
    animSize_.AddTween(sizeTween2);

    AnimationTween<float> startTweenWait(0.0f, 1.0f, 1.0f, 1.0f);

    AnimationTween<float> startTween(1.0f, startDuration_, 1.0f, 0.0f);
    startTween.SetOnFinished([this]() { isEnd_ = true; });

    animStart_.AddTween(startTweenWait);
    animStart_.AddTween(startTween);

    isInit_ = true;
}

void CountDown::Finalize()
{
    for (int i = 0; i < 3; i++)
    {
        numbers_[i]->Finalize();
    }

    start_->Finalize();
}

void CountDown::Update()
{
    if (isEnd_ || isInit_) return;

    switch (currentState_)
    {
    case State::CountDown:      UpdateCountDown();  break;
    case State::Start:          UpdateStart();      break;
    case State::End:                                break;
    };
}

void CountDown::Draw1F()
{
    if (isEnd_|| isInit_) return;

    if (!isDrawStart_)      numbers_[currentNumber_]->Draw1F();
    else                    start_->Draw1F();
}

void CountDown::Start()
{
    timer_.Reset();
    timer_.Start();
    startTimer_.Reset();
    animSize_.Start();
    animOpacity_.Start();

    isDrawStart_ = false;
    isEnd_ = false;
    isInit_ = false;
    currentNumber_ = 2;
}

void CountDown::UpdateCountDown()
{
    // 数字を変える
    if (timer_.GetNow<float>() > changeInterval_)
    {
        --currentNumber_;
        timer_.Reset();
        timer_.Start();
        animSize_.Start();
        animOpacity_.Start();
    }

    // スタート表示に変える
    if (currentNumber_ < 0)
    {
        currentState_ = State::Start;
        isDrawStart_ = true;
        currentNumber_ = 2;
        startTimer_.Reset();
        startTimer_.Start();
        animStart_.Start();
    }

    numbers_[currentNumber_]->SetColor({ 1.0f, 1.0f, 1.0f, animOpacity_.Update() });
    numbers_[currentNumber_]->SetSizeWithFactor(maxSizeFactorNumber_ * animSize_.Update());
    numbers_[currentNumber_]->Update();
}

void CountDown::UpdateStart()
{
    // 透明度を変更
    start_->SetColor({ 1.0f, 1.0f, 1.0f, animStart_.Update() });
    start_->Update();
}

#include "InGameTimer.h"
#include <Core/Window/Window.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <config/ResourcePath.h>
#include <Core/DirectX12/TextureManager.h>
#include <Math/ViewportUnits.hpp>

using namespace Math::Viewport::Unit;

void InGameTimer::Reset()
{
    isEnd_ = false;
    isNextScene_ = false;
    isStart_ = false;
    nowTime_ = 0.0;
    if (pTimer_) pTimer_->Reset();
}

void InGameTimer::CurrentTimeUpdate()
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

void InGameTimer::SpriteUpdate()
{
    double time = gameDuration_ - nowTime_;
    if (time < 0.0)
    {
        time = 0.0;
    }
    pNumericView_->SetNumber(static_cast<uint32_t>(std::ceil(time)));
    pNumericView_->Update();
}

void InGameTimer::Start()
{
    isStart_ = true;

    if (isUseSystemClock_)
    {
        pTimer_->Start();
    }
}

void InGameTimer::Initialize(bool _useSystemClock, double _gameDuration)
{
    gameDuration_ = _gameDuration;

    for (uint32_t i = 0; i < 10; ++i)
    {
        numberTextureHandles_[i] = TextureManager::GetInstance()->GetSrvHandleGPU(Path::Image::kNumbers[i]);
    }

    pNumericView_ = std::make_unique<NumericView>();
    pNumericView_->Initialize(numberTextureHandles_);
    pNumericView_->SetFontSize(64.0f);
    auto& prop = pNumericView_->GetFontLayoutProperties();
    prop.leftTop = { 50_vw, 25_vh };
    prop.anchorPoint = { 0.5f, 0.5f };

    if (_useSystemClock)
    {
        pTimer_ = std::make_unique<TimeMeasurer>();
    }

    isUseSystemClock_ = _useSystemClock;
}

void InGameTimer::Update()
{
    this->CurrentTimeUpdate();
    this->SpriteUpdate();
}

void InGameTimer::Draw1F()
{
    if (!isDisplay_)
    {
        return;
    }
    pNumericView_->Draw1F();
}

void InGameTimer::Finalize()
{
}

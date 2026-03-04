#include "InGameCountDown.h"
#include <Core/Window/Window.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <config/ResourcePath.h>
#include <Core/DirectX12/TextureManager.h>
#include <Math/ViewportUnits.hpp>
#include <cmath>

using namespace Math::Viewport::Unit;

void InGameCountDown::Reset()
{
    isEnd_ = false;
    isStart_ = false;
    nowTime_ = 0.0;
    if (pTimer_) pTimer_->Reset();
}

void InGameCountDown::CurrentTimeUpdate()
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
}

void InGameCountDown::VisualEffectUpdate()
{
    auto remainingTime = static_cast<float>(gameDuration_ - nowTime_);
    pCountDownEmphasis_->Update(*pNumericView_, remainingTime);
    pCountDownColorEmphasis_->Update(*pNumericView_, remainingTime);

    if (std::ceil(remainingTime) == 0)
    {
        isDisplay_ = false;
    }
}

void InGameCountDown::SpriteUpdate()
{
    double time = gameDuration_ - nowTime_;
    if (time < 0.0)
    {
        time = 0.0;
    }
    pNumericView_->SetNumber(static_cast<uint32_t>(std::ceil(time)));
    pNumericView_->Update();
}

void InGameCountDown::Start()
{
    isStart_ = true;

    if (isUseSystemClock_)
    {
        pTimer_->Start();
    }
}

void InGameCountDown::Initialize(bool _useSystemClock, double _gameDuration)
{
    gameDuration_ = _gameDuration;

    for (uint32_t i = 0; i < 10; ++i)
    {
        numberTextureHandles_[i] = TextureManager::GetInstance()->GetSrvHandleGPU(Path::Image::kNumbers[i]);
    }

    pNumericView_ = std::make_unique<NumericView>();
    pNumericView_->Initialize(numberTextureHandles_);
    pNumericView_->SetFontSize(kFontSize_);
    auto& prop = pNumericView_->GetFontLayoutProperties();
    prop.leftTop = { 50_vw, 25_vh };
    prop.anchorPoint = { 0.5f, 0.5f };

    if (_useSystemClock)
    {
        pTimer_ = std::make_unique<TimeMeasurer>();
    }

    isUseSystemClock_ = _useSystemClock;

    /// 強調アニメーションの初期化
    pCountDownEmphasis_ = std::make_unique<CountDownFontSizeEmphasis>();
    pCountDownEmphasis_->Initialize({ 0.0f, 5.0f }, { kFontSize_, kEmphasisFontSize_});
    pCountDownColorEmphasis_ = std::make_unique<CountDownColorEmphasis>();
    pCountDownColorEmphasis_->Initilize(kEmphasisColor_.to_Vector4(), { 0.0f, 5.0f });
    pCountDownColorEmphasis_->SetDefaultColor(kDefaultColor_.to_Vector4());
}

void InGameCountDown::Update()
{
    this->CurrentTimeUpdate();
    this->VisualEffectUpdate();
    this->SpriteUpdate();
}

void InGameCountDown::Draw1F()
{
    if (!isDisplay_)
    {
        return;
    }
    pNumericView_->Draw1F();
}

void InGameCountDown::Finalize()
{
}

#include "SpriteFadeInOut.h"



void SpriteFadeInOut::Initialize(Sprite* pSprite)
{
    pSprite_ = pSprite;
}

void SpriteFadeInOut::Play(State state, float duration)
{
    currentState_ = state;
    duration_ = duration;
}

void SpriteFadeInOut::Update()
{
    if (currentState_ == State::Standby)
    {
        return;
    }

    float alpha = 1.0f;

    /// フェードイン中は0から1へ、フェードアウト中は1から0へアルファ値を変化させる
    if (currentState_ == State::FadeIn)
    {
        alpha = std::clamp(timer_ / duration_, 0.0f, 1.0f);
    }
    else if (currentState_ == State::FadeOut)
    {
        alpha = std::clamp(1.0f - (timer_ / duration_), 0.0f, 1.0f);
    }

    /// スプライトにアルファ値を適用
    Vector4 currentColor = pSprite_->GetColor();
    pSprite_->SetColor(Vector4(currentColor.xyz(), alpha));

    /// タイマーを更新
    const float kDeltaTime = 1.0f / 60.0f;
    timer_ += kDeltaTime;
}

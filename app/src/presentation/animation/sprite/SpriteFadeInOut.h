#pragma once
#include <drawable/sprite/Sprite.h>

class SpriteFadeInOut
{
public:
    enum class State
    {
        FadeIn,
        FadeOut,
        Standby
    };

    void Initialize(Sprite* pSprite);
    void Play(State state, float duration);
    void Update();

private:
    Sprite* pSprite_ = nullptr;
    float duration_ = 0.0f;
    float timer_ = 0.0f;
    State currentState_ = State::Standby;
};
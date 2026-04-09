#include "InputAwareSprite.h"
#include <event/InputCallbackEvent.h>


void InputAwareSprite::Initialize()
{
    this->RegisterSubscriptions();
}

void InputAwareSprite::RegisterSubscriptions()
{
    subscriptions_.emplace_back() = pEventListener_->Subscribe<Events::GamePadConnected>(
        [this](const Events::GamePadConnected& e)
    {
        for (const auto& entry : entries_)
        {
            entry.pSprite_->SetTextureHandle(entry.handleGamepad_);
        }
    });

    subscriptions_.emplace_back() = pEventListener_->Subscribe<Events::GamePadDisconnected>(
        [this](const Events::GamePadDisconnected& e)
    {
        for ( const auto& entry : entries_)
        {
            entry.pSprite_->SetTextureHandle(entry.handleKeyboard_);
        }
    });
}

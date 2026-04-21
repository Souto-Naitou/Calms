#include "InputAwareSprite.h"
#include <event/InputCallbackEvent.h>
#include <Features/Input/Input.h>


void InputAwareSprite::Initialize()
{
    this->RegisterSubscriptions();
}

void InputAwareSprite::ApplyCurrentMode()
{
    if (entries_.empty()) return;

    if (Input::GetInstance()->IsPadConnected())
    {
        this->ApplyToGamepad();
    }
    else
    {
        this->ApplyToKeyboard();
    }
}

void InputAwareSprite::RegisterSubscriptions()
{
    subscriptions_.emplace_back() = pEventListener_->Subscribe<Events::GamePadConnected>(
        [this](const Events::GamePadConnected& e)
    {
        this->ApplyToGamepad();
    });

    subscriptions_.emplace_back() = pEventListener_->Subscribe<Events::GamePadDisconnected>(
        [this](const Events::GamePadDisconnected& e)
    {
        this->ApplyToKeyboard();
    });
}

void InputAwareSprite::ApplyToGamepad()
{
    for (const auto& entry : entries_)
    {
        entry.pSprite_->SetTextureHandle(entry.handleGamepad_);
        entry.pSprite_->UpdateMetadata();
        entry.pSprite_->InitializeSize();
        if (entry.sizeGamepad_) entry.pSprite_->SetSize(entry.sizeGamepad_.value());
        if (entry.positionGamepad_) entry.pSprite_->SetPosition(entry.positionGamepad_.value());
    }
}

void InputAwareSprite::ApplyToKeyboard()
{
    for (const auto& entry : entries_)
    {
        entry.pSprite_->SetTextureHandle(entry.handleKeyboard_);
        entry.pSprite_->UpdateMetadata();
        entry.pSprite_->InitializeSize();
        if (entry.sizeKeyboard_) entry.pSprite_->SetSize(entry.sizeKeyboard_.value());
        if (entry.positionKeyboard_) entry.pSprite_->SetPosition(entry.positionKeyboard_.value());
    }
}

#include "PauseLayer.h"
#include <NiGui.h>
#include <Type/NiGui_Type_Argument.h>
#include <config/ResourcePath.h>
#include <Math/ViewportUnits.hpp>
#include <Core/DirectX12/TextureManager.h>
#include <Features/DeltaTimeManager/DeltaTimeManager.h>
#include <Features/event/EventListener.h>
#include <logic/event/PauseMenuToggleEvent.h>
#include <Effects/SceneTransition/TransShutter.h>
#include <Features/SceneManager/SceneManager.h>
#include <memory>
#include <Effects/SceneTransition/TransFadeInOut.h>

using namespace Math::Viewport::Unit;

void PauseLayer::Initialize(ISceneArgs* pArgs, OrderedCanvasLayer* pLayer)
{
}

void PauseLayer::Finalize()
{
}

void PauseLayer::Update()
{
    if (!isPaused_) return;

    NiGui_Arg_Div divSettings;
    {
        auto& metadata = TextureManager::GetInstance()->GetMetaData(Path::Image::PauseMenu::kBg);
        divSettings =
        {
            .id = "pause_menu_bg",
            .textureName = Path::Image::PauseMenu::kBg,
            .color = NiVec4(1.0f, 1.0f, 1.0f, 1.0f),
            .size = NiVec2(static_cast<float>(metadata.width), static_cast<float>(metadata.height)),
            .anchor = NiGui_StandardPoint::Center,
            .pivot = NiGui_StandardPoint::Center
        };
    }
    NiGui_Arg_Button buttonSettings;
    {
        auto& metadata = TextureManager::GetInstance()->GetMetaData(Path::Image::PauseMenu::kButtonResume);
        buttonSettings =
        {
            .color = NiVec4(1.0f, 1.0f, 1.0f, 1.0f),
            .size = NiVec2(static_cast<float>(metadata.width), static_cast<float>(metadata.height)),
            .anchor = NiGui_StandardPoint::Center,
            .pivot = NiGui_StandardPoint::Center,
        };
    }

    /// バックグラウンド
    NiGui::BeginDiv(divSettings);

    {
        divSettings.id = "div_paused";
        auto& metadata = TextureManager::GetInstance()->GetMetaData(Path::Image::PauseMenu::kPaused);
        divSettings.textureName = Path::Image::PauseMenu::kPaused;
        divSettings.size = NiVec2(static_cast<float>(metadata.width), static_cast<float>(metadata.height));
        divSettings.position = NiVec2(0.0f, -vh(kMarginCenterOffsetVh_));
    }
    /// 「Paused」テキスト
    NiGui::BeginDiv(divSettings);
    NiGui::EndDiv();

    /// ボタン群
    float cursorY = vh(kMarginCenterOffsetVh_);
    buttonSettings.id = "button_resume";
    buttonSettings.textureName = Path::Image::PauseMenu::kButtonResume;
    buttonSettings.position = NiVec2(0.0f, cursorY);
    if (NiGui::Button(buttonSettings) == NiGui_ButtonState::Confirm)
    {
        EventListener::GetInstance()->Publish(PauseMenuToggleEvent());
    }

    cursorY += buttonSettings.size.y + vh(kButtonSpacingVh_);
    buttonSettings.id = "button_restart";
    buttonSettings.textureName = Path::Image::PauseMenu::kButtonRestart;
    buttonSettings.position = NiVec2(0.0f, cursorY);
    if (NiGui::Button(buttonSettings) == NiGui_ButtonState::Confirm)
    {
        SceneManager::GetInstance()->ReserveScene("GameScene", "LoadingScreen", std::make_unique<TransFadeInOut>());
        EventListener::GetInstance()->Publish(PauseMenuToggleEvent());
    }

    cursorY += buttonSettings.size.y + vh(kButtonSpacingVh_);
    buttonSettings.id = "button_back_to_title";
    buttonSettings.textureName = Path::Image::PauseMenu::kButtonBackToTitle;
    buttonSettings.position = NiVec2(0.0f, cursorY);
    if (NiGui::Button(buttonSettings) == NiGui_ButtonState::Confirm)
    {
        SceneManager::GetInstance()->ReserveScene("TitleScene", std::make_unique<TransShutter>());
        EventListener::GetInstance()->Publish(PauseMenuToggleEvent());
    }

    NiGui::EndDiv();
}

void PauseLayer::Draw()
{
}

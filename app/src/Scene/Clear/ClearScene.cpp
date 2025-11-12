#include "ClearScene.h"

#include <Core/Win32/WinSystem.h>
#include <Features/Input/Input.h>

#include <Features/SceneManager/SceneManager.h>
#include <Effects/SceneTransition/TransFadeInOut.h>
#include <Features/Layer/CanvasScope.h>

void ClearScene::Initialize()
{
    pSpace_ = std::make_unique<Sprite>();
    pSpace_->Initialize("spacePress.png");
    pSpace_->SetName("SpaceSprite");
    pSpace_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpace_->SetPosition({ WinSystem::clientWidth / 2, WinSystem::clientHeight / 2 + 100 });

    pClear_ = std::make_unique<Sprite>();
    pClear_->Initialize("clear.png");
    pClear_->SetName("ClearSprite");
    pClear_->SetAnchorPoint({ 0.5f, 0.5f });
    pClear_->SetPosition({ WinSystem::clientWidth / 2, WinSystem::clientHeight / 2 });

    Canvas::Params canvasParams = {};
    canvasParams.name = "ClearSceneUI";
    canvasParams.pDx12 = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));
    #ifdef _DEBUG
    canvasParams.pImGuiManager = std::any_cast<ImGuiManager*>(pArgs_->Get("ImGuiManager"));
    #endif // _DEBUG

    canvasUI_ = std::make_unique<Canvas>();
    canvasUI_->Initialize(canvasParams);

    pLayer_->AddCanvas(canvasUI_.get());
}

void ClearScene::Finalize()
{
    pClear_->Finalize();
    pSpace_->Finalize();

    canvasUI_->Finalize();
    pLayer_->RemoveCanvas(canvasUI_.get());
}

void ClearScene::Update()
{
    pClear_->Update();
    pSpace_->Update();

    if (Input::GetInstance()->TriggerKey(DIK_SPACE))
    {
        SceneManager::GetInstance()->ReserveScene("TitleScene", std::make_unique<TransFadeInOut>());
    }
}

void ClearScene::Draw()
{
    CanvasScope canvasScope(canvasUI_.get());
    pClear_->Draw1F();
    pSpace_->Draw1F();
}

void ClearScene::DrawTexts()
{
}

#include "LoadingScreen.h"
#include "config/ResourcePath.h"
#include <Core/Window/Window.h>
#include <filesystem>
#include <Core/ConfigManager/ConfigManager.h>
#include <cctype>
#include <vector>
#include <Effects/SceneTransition/TransShutter.h>
#include <Features/SceneManager/SceneManager.h>
#include <Features/Layer/CanvasScope.h>


void LoadingScreen::Initialize()
{
    pInput_ = Input::GetInstance();

    DirectX12* pDx12 = std::any_cast<DirectX12*>(pArgs_->Get("DirectX12"));
    CubemapSystem* pCubemapSystem = std::any_cast<CubemapSystem*>(pArgs_->Get("CubemapSystem"));

    this->InitializeCanvas(pDx12, pCubemapSystem);
    this->InitializeDrawables();

    waitTimer_.Start();
}

void LoadingScreen::Finalize()
{
    pBar_->Finalize();
    pCanvas_->Finalize();
    pLayer_->RemoveCanvas(pCanvas_.get());
}

void LoadingScreen::Update()
{
    if (waitTimer_.GetNow<float>() >= kWaitTime_ && !isTexturePathAggregated_)
    {
        auto& cfg = ConfigManager::GetInstance()->GetConfigData();
        this->AggregateTexturePaths(cfg.texture_paths.front());
        this->AggregateTexturePaths(cfg.model_paths.front());
        pBar_->SetMaxValue(static_cast<float>(taskExecutor_.GetCount()));
        isTexturePathAggregated_ = true;
    }

    /// パスリストが残っていれば
    if (isTexturePathAggregated_ && taskExecutor_.GetCount())
    {
        taskExecutor_.ExecuteOrdered();
        current_ += 1.0f;
    }
    
    // スムーズに値を変化させる
    const float barValue = pBar_->GetCurrentValue();
    float smoothValue = barValue + (current_ - barValue) * kSmoothFactor_;

    pBar_->SetCurrentValue(smoothValue);

    pSpriteLBackground_->Update();
    pSpriteLoading_->Update();
    pBar_->Update();
}

void LoadingScreen::Draw()
{
    CanvasScope canvasScope(pCanvas_.get());
    pSpriteLBackground_->Draw1F();
    pSpriteLoading_->Draw1F();
    pBar_->Draw1F();
}

bool LoadingScreen::IsEnd() const
{
    return pBar_->GetCurrentValue() >= pBar_->GetMaxValue() - 0.1f;
}

void LoadingScreen::AggregateTexturePaths(const std::string& directoryPath)
{
    std::filesystem::path dirPath(directoryPath);
    if (!std::filesystem::exists(dirPath) || !std::filesystem::is_directory(dirPath))
    {
        return;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dirPath))
    {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        if (ext == ".png" || ext == ".dds" || ext == ".jpg" || ext == ".jpeg")
        {
            taskExecutor_.AddTask([=]()
            {
                pTextureManager_->LoadTexture(entry.path().string());
            });
        }
    }
}

void LoadingScreen::InitializeDrawables()
{
    /// ローディングスプライトの初期化
    pTextureManager_ = TextureManager::GetInstance();
    pSpriteLoading_ = std::make_unique<Sprite>();
    pSpriteLoading_->Initialize(Path::Image::kLoading);
    pSpriteLoading_->SetName("Loading");
    pSpriteLoading_->SetAnchorPoint({ 1.0f, 0.5f });
    pSpriteLoading_->SetPosition({ Window::clientWidth - Window::clientWidth / 12.8f, Window::clientHeight - 80.0f });
    pSpriteLoading_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });
    pSpriteLoading_->SetSizeWithFactor(Window::clientWidth / 1600.0f);

    /// ローディング背景スプライトの初期化
    pSpriteLBackground_ = std::make_unique<Sprite>();
    pSpriteLBackground_->Initialize("white1x1.png");
    pSpriteLBackground_->SetName("LoadingBackground");
    pSpriteLBackground_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f });
    pSpriteLBackground_->SetSize({ Window::clientWidth, Window::clientHeight });

    /// ローディングバーの初期化
    Bar2dInitParams barParams = {};
    barParams.barSize = { Window::clientWidth / 2.0f, Window::clientHeight / 30.0f };
    pBar_ = std::make_unique<Bar2d>();
    pBar_->Initialize(barParams);
    pBar_->SetAnchorPoint({ 1.0f, 0.5f });
    pBar_->SetPosition({ Window::clientWidth - Window::clientWidth / 2.5f, Window::clientHeight - 80.0f });
    pBar_->SetCurrentValue(0.0f);
}

void LoadingScreen::InitializeCanvas(DirectX12* pDx12, CubemapSystem* pCubemapSystem)
{
    Canvas::Params canvasParams = {};
    canvasParams.name = "LoadingScreenCanvas";
    canvasParams.pDx12 = pDx12;
    canvasParams.pCubemapSystem = pCubemapSystem;
    #ifdef _DEBUG
    canvasParams.pImGuiManager = std::any_cast<ImGuiManager*>(pArgs_->Get("ImGuiManager"));
    #endif // _DEBUG
    pCanvas_ = std::make_unique<Canvas>();
    pCanvas_->Initialize(canvasParams);
    pLayer_->AddCanvas(pCanvas_.get());
}

#include "LoadScene.h"
#include "config/ResourcePath.h"
#include <Core/Win32/WinSystem.h>
#include <filesystem>
#include <Core/ConfigManager/ConfigManager.h>
#include <cctype>
#include <vector>
#include <Effects/SceneTransition/TransShutter.h>
#include <Features/SceneManager/SceneManager.h>


void LoadScene::Initialize()
{
    pInput_ = Input::GetInstance();

    pTextureManager_ = TextureManager::GetInstance();
    pSpriteLoading_ = std::make_unique<Sprite>();
    pSpriteLoading_->Initialize(Path::Image::kLoading);
    pSpriteLoading_->SetName("Loading");
    pSpriteLoading_->SetAnchorPoint({ 1.0f, 0.5f });
    pSpriteLoading_->SetPosition({ WinSystem::clientWidth - 130.0f, WinSystem::clientHeight - 80.0f});
    pSpriteLoading_->SetColor({ 0.2f, 0.2f, 0.2f, 1.0f });

    pBar_ = std::make_unique<Bar2d>();
    pBar_->Initialize("white1x1.png", {800.0f, 30.0f});
    pBar_->SetAnchorPoint({ 1.0f, 0.5f });
    pBar_->SetPosition({ WinSystem::clientWidth - 600.0f, WinSystem::clientHeight - 80.0f });
    pBar_->SetCurrentValue(0.0f);

    pSpriteLBackground_ = std::make_unique<Sprite>();
    pSpriteLBackground_->Initialize("white1x1.png");
    pSpriteLBackground_->SetName("LoadingBackground");
    pSpriteLBackground_->SetColor({ 0.8f, 0.8f, 0.8f, 1.0f });
    pSpriteLBackground_->SetSize({ WinSystem::clientWidth, WinSystem::clientHeight });

    waitTimer_.Start();
}

void LoadScene::Finalize()
{

}

void LoadScene::Update()
{
    if (waitTimer_.GetNow<float>() >= kWaitTime_ && !isAggregated_)
    {
        auto& cfg = ConfigManager::GetInstance()->GetConfigData();
        this->AggregateTexturePaths(cfg.texture_paths.front());
        this->AggregateTexturePaths(cfg.model_paths.front());
        pBar_->SetMaxValue(static_cast<float>(texturePaths_.size()));
        isAggregated_ = true;
    }

    if (isAggregated_ && !texturePaths_.empty())
    {
        pTextureManager_->LoadTexture(texturePaths_.back());
        texturePaths_.pop_back();
        current_ += 1.0f;
    }
    
    // スムーズに値を変化させる
    const float barValue = pBar_->GetCurrentValue();
    float smoothValue = barValue + (current_ - barValue) * kSmoothFactor_;

    pBar_->SetCurrentValue(smoothValue);

    if (isAggregated_ && pBar_->GetCurrentValue() >= pBar_->GetMaxValue() - 0.1f && !isChangingScene_)
    {
        SceneManager::GetInstance()->ReserveScene("GameScene", std::make_unique<TransShutter>());
        isChangingScene_ = true;
    }

    pSpriteLBackground_->Update();
    pSpriteLoading_->Update();
    pBar_->Update();
}

void LoadScene::Draw()
{
    pSpriteLBackground_->Draw();
    pSpriteLoading_->Draw();
    pBar_->Draw2D();
}

void LoadScene::AggregateTexturePaths(const std::string& directoryPath)
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
            texturePaths_.push_back(entry.path().string());
        }
    }
}

#include "ScoreCalculator.h"
#include <Config/ResourcePath.h>
#include <format>
#include <Core/DirectX12/TextureManager.h>
#include <Core/Win32/WinSystem.h>

void ScoreCalculator::Initialize()
{
    /// スコアの初期化
    score_ = 0;
    enemyDeathCount_ = 0;

    // デバッグエントリの作成
    pDebugEntry_ = std::make_unique<DebugEntry<ScoreCalculator>>(
        "ScoreCalculator", this, false
    );

    this->GetTextureHandles();
    this->InitializeSprites();
}

void ScoreCalculator::Update()
{
    this->ApplyScoreToSprites();

    this->UpdateDisplayScore();

    this->UpdateSprites();
}

void ScoreCalculator::Draw1F()
{
    for (const auto& digitSprite : scoreDigits_)
    {
        digitSprite->Draw1F();
    }
}

void ScoreCalculator::Finalize()
{
    for (const auto& digitSprite : scoreDigits_)
    {
        digitSprite->Finalize();
    }
}

void ScoreCalculator::CountEnemyDeath()
{
    ++enemyDeathCount_;
    receiveAddScore_ += ScorePerUnit::kEnemy;
}

void ScoreCalculator::ImGui()
{
    if (ImGui::DragFloat2("LeftTop", &scoreLeftTop_.x, 0.01f, 0.0f, FLT_MAX))
    {
        for (uint32_t i = 0; i < scoreDigits_.size(); ++i)
        {
            UpdatePosition(i);
        }
    }

    if (ImGui::DragFloat("FontWidth", &fontWidth_, 0.01f, 1.0f, FLT_MAX))
    {
        for (uint32_t i = 0; i < scoreDigits_.size(); ++i)
        {
            UpdateFontWidth(scoreDigits_[i].get());
            UpdatePosition(i);
        }
    }

    if (ImGui::DragFloat("LetterSpacing", &letterSpacing_, 0.01f, 0.0f, FLT_MAX))
    {
        for (uint32_t i = 0; i < scoreDigits_.size(); ++i)
        {
            UpdatePosition(i);
        }
    }
}

void ScoreCalculator::GetTextureHandles()
{
    // 0~9のテクスチャハンドルを取得
    for (uint32_t i = 0; i < 10u; ++i)
    {
        auto& filepath = Path::Image::kNumbers[i];
        digitTextureHandles_[i] = TextureManager::GetInstance()->GetSrvHandleGPU(filepath);
    }
}

void ScoreCalculator::InitializeSprites()
{
    constexpr static uint32_t marginLeft = 32u;
    scoreLeftTop_ =
    {
        static_cast<float>(marginLeft),
        static_cast<float>(WinSystem::clientHeight) / 2.0f
    };

    for (uint32_t i = 0; i < scoreDigits_.size(); ++i)
    {
        auto& digitSprite = scoreDigits_[i];
        
        // メタデータ取得
        const auto& metadata = TextureManager::GetInstance()->GetMetaData(Path::Image::kNumbers[i]);
        
        digitSprite = std::make_unique<Sprite>();
        digitSprite->Initialize(digitTextureHandles_.front());
        digitSprite->SetAnchorPoint({ 0.0f, 0.5f });

        UpdateFontWidth(digitSprite.get());
        UpdatePosition(i);
    }
}

void ScoreCalculator::UpdateSprites()
{
    for (uint32_t i = 0; i < scoreDigits_.size(); ++i)
    {
        scoreDigits_[i]->Update();
    }
}

void ScoreCalculator::UpdateDisplayScore()
{
    /// スコア表示を徐々に加算する
    float addScore = 0;
    addScore = receiveAddScore_ / static_cast<float>(scoreIncrementPerFrame_);
    receiveAddScore_ -= addScore;
    score_ += addScore;
}

void ScoreCalculator::ApplyScoreToSprites()
{
    auto displayScore = static_cast<uint32_t>(score_);
    for (int32_t i = static_cast<int32_t>(scoreDigits_.size()) - 1; i >= 0; --i)
    {
        const uint32_t digit = displayScore % 10u;
        displayScore /= 10u;
        scoreDigits_[i]->SetTextureHandle(digitTextureHandles_[digit]);
    }
}

void ScoreCalculator::UpdatePosition(uint32_t index)
{
    Vector2 position = scoreLeftTop_;
    position.x += static_cast<float>(index) * (fontWidth_ + letterSpacing_);
    scoreDigits_[index]->SetPosition(position);
}

void ScoreCalculator::UpdateFontWidth(Sprite* sprite) const
{
    Vector2 size = sprite->GetSize();
    float aspectRatio = size.x / size.y;
    size.x = static_cast<float>(fontWidth_);
    size.y = static_cast<float>(fontWidth_) / aspectRatio;
    sprite->SetSize(size);
}

#include "ScoreCalculator.h"
#include <Config/ResourcePath.h>
#include <format>
#include <Core/DirectX12/TextureManager.h>
#include <Math/ViewportUnits.hpp>
#include <Features/event/EventListener.h>
#include <logic/event/KillEnemyEvent.h>

void ScoreCalculator::Initialize()
{
    /// スコアの初期化
    score_ = 0;
    enemyDeathCount_ = 0;

    this->InitializeScoreTable();
    this->InitializeNumericView();
    subKillEnemy_ = EventListener::GetInstance()->Subscribe<KillEnemyEvent>([this](const KillEnemyEvent& e)
    {
        this->CountEnemyDeath(e.enemyType);
    });
}

void ScoreCalculator::Update()
{
    this->UpdateDisplayScore();

    this->UpdateNumericView();
}

void ScoreCalculator::Draw1F()
{
    pScore_->Draw1F();
}

void ScoreCalculator::Finalize()
{
}

void ScoreCalculator::CountEnemyDeath(EnemyType type)
{
    ++enemyDeathCount_;
    receiveAddScore_ += scoreTable_[type];
}

void ScoreCalculator::InitializeScoreTable()
{
    scoreTable_[EnemyType::Normal] = ScorePerUnit::kEnemyNormal;
    scoreTable_[EnemyType::Rusher] = ScorePerUnit::kEnemyRusher;
}

void ScoreCalculator::InitializeNumericView()
{
    std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 10u> textureHandles{};
    // 0~9のテクスチャハンドルを取得
    for (uint32_t i = 0; i < 10u; ++i)
    {
        auto& filepath = Path::Image::kNumbers[i];
        textureHandles[i] = TextureManager::GetInstance()->GetSrvHandleGPU(filepath);
    }

    constexpr static uint32_t marginLeft = 32u;
    scoreLeftTop_ =
    {
        static_cast<float>(marginLeft),
        50.0_vh
    };

    pScore_ = std::make_unique<NumericView>();
    pScore_->Initialize(textureHandles);
    pScore_->SetFontSize(kFontHeight_);
    auto& fontLayoutProps = pScore_->GetFontLayoutProperties();
    fontLayoutProps.leftTop = scoreLeftTop_;
    fontLayoutProps.anchorPoint = { 0.0f, 0.5f };
}

void ScoreCalculator::UpdateNumericView()
{
    pScore_->SetNumber(static_cast<uint32_t>(score_));
    pScore_->Update();
}

void ScoreCalculator::UpdateDisplayScore()
{
    /// スコア表示を徐々に加算する
    float addScore = 0;
    addScore = receiveAddScore_ / static_cast<float>(scoreIncrementPerFrame_);
    receiveAddScore_ -= addScore;
    score_ += addScore;
}

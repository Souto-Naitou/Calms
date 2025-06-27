#include "ScoreSystem.h"

#include <format>

void ScoreSystem::Initialize()
{
    /// スコアの初期化
    score_ = 0;
    enemyDeathCount_ = 0;

    pScore_ = std::make_unique<Text>();
    pScore_->Initialize();
    pScore_->SetName("Score");
    pScore_->SetAnchorPoint(TextStandardPoint::CenterLeft);
    pScore_->SetPivot(TextStandardPoint::CenterLeft);
    pScore_->SetMaxSize({ 400, 100 });
    pScore_->SetFontSize(48);
    pScore_->SetColorName("HarfWhite");
    pScore_->SetText("0");
    pScore_->SetFontFamily("Bahnschrift");

    pName_ = std::make_unique<Text>();
    pName_->Initialize();
    pName_->SetName("ScoreName");
    pName_->SetParent(pScore_.get());
    pName_->SetPosition({ 0, -6 });
    pName_->SetPivot(TextStandardPoint::Center);
    pName_->SetMaxSize({ 400, 100 });
    pName_->SetFontSize(32);
    pName_->SetColorName("HarfWhite");
    pName_->SetText("Score:");
    pName_->SetFontFamily("Bahnschrift");
}

void ScoreSystem::Update()
{
    pScore_->SetText(std::format("{:010}", static_cast<int>(score_)));

    float addScore = 0;
    addScore = receiveAddScore_ / static_cast<float>(framesAddScore_);
    receiveAddScore_ -= addScore;

    score_ += addScore;

    
    pScore_->SetPosition(Vector2(pName_->GetSize().x / 2.0f, 0.0f));

    pName_->Update();
    pScore_->Update();
}

void ScoreSystem::DrawTxt()
{
    pName_->Draw();
    pScore_->Draw();
}

void ScoreSystem::Finalize()
{
    pName_->Finalize();
    pScore_->Finalize();
}

void ScoreSystem::CountEnemyDeath()
{
    enemyDeathCount_++;
    receiveAddScore_ += kEnemyScore;
}

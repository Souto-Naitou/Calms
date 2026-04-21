#include "ScoreEvaluationView.h"
#include <Core/DirectX12/TextureManager.h>
#include <config/ResourcePath.h>
#include <Math/ViewportUnits.hpp>

using namespace Math::Viewport::Unit;

void ScoreEvaluationView::Initialize()
{
    pSpriteResult_ = std::make_unique<Sprite>();
}

void ScoreEvaluationView::SetResult(ScoreReviewer::Result result)
{
    std::string path = Path::Image::Clear::kScoreResult[static_cast<size_t>(result)];
    pSpriteResult_->Initialize(path);
    pSpriteResult_->SetColor(Vector4{ 1.0f, 1.0f, 1.0f, 0.0f });
    pSpriteResult_->SetPosition({ 25_vw, 44_vh });
    pSpriteResult_->SetAnchorPoint({ 0.5f, 0.5f });
    pSpriteResult_->SetSizeWithFactor(0.5f);
    isSetResult_ = true;
}

void ScoreEvaluationView::Update()
{
    if (!isSetResult_) return;
    pSpriteResult_->Update();
}

void ScoreEvaluationView::Draw1F()
{
    if (!isSetResult_) return;
    pSpriteResult_->Draw1F();
}

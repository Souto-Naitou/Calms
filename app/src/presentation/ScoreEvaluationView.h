#pragma once
#include <memory>
#include <drawable/sprite/Sprite.h>
#include <logic/score/ScoreReviewer.h>

class ScoreEvaluationView
{
public:
    void Initialize();
    void SetResult(ScoreReviewer::Result result);
    void Update();
    void Draw1F();

    Sprite* GetSprite() { return pSpriteResult_.get(); }

private:
    bool isSetResult_ = false;
    std::unique_ptr<Sprite> pSpriteResult_ = nullptr;  // !< 結果スプライト
    D3D12_GPU_DESCRIPTOR_HANDLE resultTextureHandle_ = {};  // !< 結果テクスチャハンドル 
};
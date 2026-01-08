#pragma once
#include <Features/Text/Text.h>
#include <memory>
#include <drawable/sprite/Sprite.h>
#include <array>


namespace ScorePerUnit
{
    static constexpr auto kEnemy = 100u;
}

/// <summary>
/// スコア計算クラス
/// </summary>
class ScoreCalculator
{
public:
    /// <summary>
    /// スコア表示用テキスト等を初期化します。
    /// </summary>
    void Initialize();

    /// <summary>
    /// スコアの加算アニメーション等の更新を行います。
    /// </summary>
    void Update();

    /// <summary>
    /// スコアのテキスト描画を行います。
    /// </summary>
    void Draw1F();

    /// <summary>
    /// 終了処理を行います。
    /// </summary>
    void Finalize();

    /// <summary>
    /// 敵撃破数をカウントし、スコア加算を行います。
    /// </summary>
    void CountEnemyDeath();

    /// <summary>
    /// デバッグUIを描画します。
    /// </summary>
    void ImGui();

private:
    void GetTextureHandles();
    void InitializeSprites();
    void UpdateSprites();
    void UpdateDisplayScore();
    // スコアをスプライトに反映します。
    void ApplyScoreToSprites();
    void UpdatePosition(uint32_t index);
    void UpdateFontWidth(Sprite* sprite) const;

    constexpr static uint32_t numDigits                 = 8u;       // スコア最大桁数
                     float    letterSpacing_            = 0.0f;     // 文字間隔
                     float    fontWidth_                = 48.0f;    // フォント幅
                     uint32_t scoreIncrementPerFrame_   = 20u;
                     Vector2  scoreLeftTop_             = {};

    // デバッグエントリ
    std::unique_ptr<DebugEntry<ScoreCalculator>> pDebugEntry_ = nullptr;

    /// メンバー変数
    float           score_              = 0.0f;
    unsigned int    enemyDeathCount_    = 0u;
    float           receiveAddScore_    = 0.0f;

    // 毎フレーム加算するスコア量
    std::unique_ptr<Text> pName_    = nullptr;
    std::unique_ptr<Text> pScore_   = nullptr;

    std::array<std::unique_ptr<Sprite>, numDigits>  scoreDigits_ = {};
    std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 10>     digitTextureHandles_ = {};
};
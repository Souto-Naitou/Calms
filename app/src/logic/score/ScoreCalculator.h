#pragma once
#include <Features/Text/Text.h>
#include <memory>
#include <drawable/font/NumericView.h>


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

private:
    void InitializeNumericView();
    void UpdateNumericView();
    void UpdateDisplayScore();
    
    constexpr static uint32_t kNumDigits_               = 8u;       // スコア最大桁数
    constexpr static float    kFontHeight_              = 48.0f;    // フォント幅
                     uint32_t scoreIncrementPerFrame_   = 20u;
                     Vector2  scoreLeftTop_             = {};

    /// メンバー変数
    float           score_              = 0.0f;
    unsigned int    enemyDeathCount_    = 0u;
    float           receiveAddScore_    = 0.0f;

    // 毎フレーム加算するスコア量
    std::unique_ptr<Text>           pName_    = nullptr;
    std::unique_ptr<NumericView>    pScore_   = nullptr;
};
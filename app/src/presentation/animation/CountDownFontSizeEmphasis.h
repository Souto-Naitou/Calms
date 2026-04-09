#pragma once
#include <Features/Animation/AnimationTimeline.hpp>
#include <Range.h>
#include <drawable/font/NumericView.h>

/// NumericViewのコンポーネント
/// カウントダウンの残り時間が特定の範囲内に入ったときに、フォントサイズを強調するアニメーションを再生するためのクラス
class CountDownFontSizeEmphasis
{
public:
    void Initialize(Range<float> targetTimeRange, Range<float> fontSizeRange);
    void Update(NumericView& nv, float remainingTime);

private:
    Range<float>                targetTimeRange_    = {};
    Range<float>                fontSizeRange_      = {};
    AnimationTimeline<float>    timeline_           = {};

    /// 一時的な状態を保持するための変数
    bool                        isPlaying_      = false;
    int                         timeSec_        = 0;
};
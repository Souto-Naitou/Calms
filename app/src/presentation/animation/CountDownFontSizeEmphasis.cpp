#include "CountDownFontSizeEmphasis.h"
#include <Math/Easing.h>
#include <cmath>



void CountDownFontSizeEmphasis::Initialize(Range<float> targetTimeRange, Range<float> fontSizeRange)
{
    AnimationTween<float> tween0(0.0f, 0.1f, 0.0f, 1.0f);
    tween0.SetTransitionFunction(&Math::Easing::EaseInQuad);
    AnimationTween<float> tween1(0.1f, 0.9f, 1.0f, 0.0f);
    tween1.SetTransitionFunction(&Math::Easing::EaseOutQuad);
    timeline_.ClearTween();
    timeline_.AddTween(tween0);
    timeline_.AddTween(tween1);
    targetTimeRange_ = targetTimeRange;
    fontSizeRange_ = fontSizeRange;
}

void CountDownFontSizeEmphasis::Update(NumericView& nv, float remainingTime)
{
    // 秒数が指定の範囲内 かつ
    // アニメーションが再生されていなかったら
    // アニメーションを開始する
    if (targetTimeRange_.Contains(remainingTime) && !isPlaying_)
    {
        isPlaying_ = true;
        timeline_.Start();
    }

    /// 秒数が変わったら強調アニメーションを再生する
    int timeSecPrev = timeSec_;
    timeSec_ = static_cast<int>(std::floor(remainingTime));
    if (timeSecPrev != timeSec_)
    {
        isPlaying_ = false;
    }

    /// アニメーションの更新
    float t = timeline_.Update();
    // フォントサイズを補間して設定する
    float fontSize = std::lerp(fontSizeRange_.start, fontSizeRange_.end, t);
    nv.SetFontSize(fontSize);
}

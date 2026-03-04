#pragma once

#include <Range.h>
#include <Vector4.h>
#include <drawable/font/NumericView.h>

class CountDownColorEmphasis
{
public:
    void Initilize(const Vector4& color, Range<float> targetTimeRange);
    void Update(NumericView& nv, float remainingTime);

    /// デフォルトカラーの設定
    /// 元の色に戻したい際に設定してください。
    void SetDefaultColor(const Vector4& color) { defaultColor_ = color; }

private:
    Range<float>            targetTimeRange_    = {};
    Vector4                 colorEmphasis_      = {};

    // デフォルトカラー（optional）
    std::optional<Vector4>  defaultColor_       = std::nullopt;
};
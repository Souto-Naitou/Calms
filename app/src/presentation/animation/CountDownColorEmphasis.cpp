#include "CountDownColorEmphasis.h"

void CountDownColorEmphasis::Initilize(const Vector4& color, Range<float> targetTimeRange)
{
    targetTimeRange_ = targetTimeRange;
    colorEmphasis_ = color;
}

void CountDownColorEmphasis::Update(NumericView& nv, float remainingTime)
{
    if (targetTimeRange_.Contains(remainingTime))
    {
        nv.SetColor(colorEmphasis_);
    }
    else if (defaultColor_)
    {
        nv.SetColor(*defaultColor_);
    }
}

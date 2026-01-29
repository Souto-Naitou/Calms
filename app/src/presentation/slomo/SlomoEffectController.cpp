#include "SlomoEffectController.h"
#include <cmath>
#include <mathExtension.h>

SlomoEffectController::Result SlomoEffectController::Update(bool isSlomoActive, const Context& currentCtx)
{
    const auto& kGameEyeFollowRateDuringSlow    = config_.gameEyeFollowRateDuringSlow;
    const auto& kGameEyeFollowRateNormal        = config_.gameEyeFollowRateNormal;
    const auto& kGrayscalePowerDuringSlow       = config_.grayscalePowerDuringSlow;
    const auto& kGrayscaleBlendRateDuringSlow   = config_.grayscaleBlendRateDuringSlow;
    const auto& kGrayscaleBlendRateNormal       = config_.grayscaleBlendRateDuringSlow;

    Result result{};

    const auto& kPlayerPos      = currentCtx.playerPosition;
    const auto& kEyePos         = currentCtx.gameEyePosition;
    const auto& kGrayScalePower = currentCtx.grayscalePower;

    if (isSlomoActive)
    {
        /// [ カメラをプレイヤーに近づける ]
        auto eyeTarget = Vector3(kPlayerPos.x, config_.gameEyeHeightDuringSlow, kPlayerPos.z);
        result.gameEyePosition = Math::Lerp(kEyePos, eyeTarget, kGameEyeFollowRateDuringSlow);

        /// [ グレースケールエフェクトの強さを変える (0<) ]
        result.grayscalePower = std::lerp(
            kGrayScalePower,
            kGrayscalePowerDuringSlow,
            kGrayscaleBlendRateDuringSlow);
    }
    else
    {
        /// [ カメラをデフォルト位置に戻す ]
        auto eyeTarget = Vector3(kPlayerPos.x, config_.gameEyeHeightDefault, kPlayerPos.z);
        result.gameEyePosition = Math::Lerp(kEyePos, eyeTarget, kGameEyeFollowRateNormal);

        /// [ グレースケールエフェクトの強さを変える (0>) ]
        result.grayscalePower = std::lerp(kGrayScalePower, 0.0f, kGrayscaleBlendRateNormal);
    }

    return result;
}

#pragma once
#include <Vector3.h>

class SlomoEffectController
{
public:
    struct Result
    {
        Vector3 gameEyePosition;
        float   grayscalePower;
    };
    struct Config
    {
        float gameEyeHeightDefault          = 65.0f;
        float gameEyeHeightDuringSlow       = 30.0f;
        float gameEyeFollowRateDuringSlow   = 0.1f;
        float gameEyeFollowRateNormal       = 0.1f;
        float grayscalePowerDuringSlow      = 0.75f;
        float grayscaleBlendRateDuringSlow  = 0.1f;
    };
    struct Context
    {
        Vector3 playerPosition  = {};
        Vector3 gameEyePosition = {};
        float   grayscalePower  = 0.0f;
    };

    SlomoEffectController() = default;

    void SetConfig(const Config& config)
    {
        config_ = config;
    }

    /// <summary>
    /// スロモエフェクトの更新を行います。
    /// </summary>
    /// <param name="isSlomoActive">スロモがアクティブかどうか。</param>
    Result Update(bool isSlomoActive, const Context& currentCtx);

private:
    Config config_;
};
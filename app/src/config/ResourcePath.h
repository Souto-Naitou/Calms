#pragma once

/// <summary>
/// 画像リソースのパスを定義する名前空間
/// </summary>
namespace Path::Image
{
    /// [ 共通 ]
    inline constexpr const char* kWhite = "white1x1.png";
    inline constexpr const char* kNumbers[] =
    {
        "common/font/number_0.png",
        "common/font/number_1.png",
        "common/font/number_2.png",
        "common/font/number_3.png",
        "common/font/number_4.png",
        "common/font/number_5.png",
        "common/font/number_6.png",
        "common/font/number_7.png",
        "common/font/number_8.png",
        "common/font/number_9.png",
    };
    inline constexpr const char* kCountDownEndString = "common/font/start.png";
    inline constexpr const char* kSpaceText = "common/font/space.png";
    inline constexpr const char* kClearText = "common/font/clear.png";

    /// [ パーティクル ]
    inline constexpr const char* kParticleCircle = "particle/circle_256x256_white.png";

    /// [ タイトルシーン ]
    inline constexpr const char* kTitle = "title/title.png";
    inline constexpr const char* kTitleSkybox = "title/skybox.v2.dds";
    inline constexpr const char* kTitleStartPrompt = "title/start_prompt.png";
    inline constexpr const char* kFrameScreen = "title/frame_screen.png";

    /// [ ローディング画面 ]
    inline constexpr const char* kLoading = "load/NOWLOADING.png";

    /// [ ゲームシーン ]
    // ガイド画像
    inline constexpr const char* kGuideShoot = "guide/leftclick.png";
    inline constexpr const char* kGuideSlow = "guide/shift.png";
    // レティクル
    inline constexpr const char* kReticle = "reticle.png";
}

namespace Path::Audio
{
    inline constexpr const char* kSeStartButton = "se_title_startbutton.wav";
    inline constexpr const char* kSePlayerShoot = "se_player_shoot.wav";
    inline constexpr const char* kSePlayerDeath = "se_player_death.wav";
    inline constexpr const char* kSePlayerSlowOn = "se_player_slow_on.wav";
    inline constexpr const char* kSePlayerSlowOff = "se_player_slow_off.wav";
    inline constexpr const char* kSeEnemyDeath = "se_enemy_death.wav";
}
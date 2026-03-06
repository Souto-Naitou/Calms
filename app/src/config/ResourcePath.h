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
        "common/font/number_0.dds",
        "common/font/number_1.dds",
        "common/font/number_2.dds",
        "common/font/number_3.dds",
        "common/font/number_4.dds",
        "common/font/number_5.dds",
        "common/font/number_6.dds",
        "common/font/number_7.dds",
        "common/font/number_8.dds",
        "common/font/number_9.dds",
    };
    inline constexpr const char* kCountDownEndString = "common/font/start.dds";
    inline constexpr const char* kSpaceText = "common/font/space.dds";
    inline constexpr const char* kClearText = "common/font/clear.dds";

    /// [ パーティクル ]
    inline constexpr const char* kParticleCircle = "particle/circle_256x256_white.dds";

    /// [ タイトルシーン ]
    inline constexpr const char* kTitle = "title/title.dds";
    inline constexpr const char* kTitleSkybox = "title/skybox.v2.dds";
    inline constexpr const char* kTitleStartPrompt = "title/start_prompt.dds";
    inline constexpr const char* kFrameScreen = "title/frame_screen.dds";

    /// [ ローディング画面 ]
    inline constexpr const char* kLoading = "load/NOWLOADING.dds";

    /// [ ゲームシーン ]
    namespace InGame
    {
        // ガイド画像
        inline constexpr const char* kGuideShoot = "guide/shoot.dds";
        inline constexpr const char* kGuideSlow = "guide/slomo.dds";
        inline constexpr const char* kGuideExplosion = "guide/explosion.dds";
        inline constexpr const char* kGuidePause = "guide/pause.dds";
        // レティクル
        inline constexpr const char* kReticle = "reticle.dds";
        // 爆発エンティティ
        inline constexpr const char* kExplosionRing = "game/player_explosion.dds";
    }


    /// [ ポーズメニュー ]
    namespace PauseMenu
    {
        inline constexpr const char* kBg = "game/pause/bg.dds";
        inline constexpr const char* kPaused = "game/pause/paused.dds";
        inline constexpr const char* kButtonResume = "game/pause/button_resume.dds";
        inline constexpr const char* kButtonRestart = "game/pause/button_retry.dds";
        inline constexpr const char* kButtonBackToTitle = "game/pause/button_back_to_title.dds";
    }
}

namespace Path::Audio
{
    /// [ BGM ]
    inline constexpr const char* kBgmTitle = "bgm_title.wav";
    inline constexpr const char* kBgmInGame = "bgm_ingame.wav";

    /// [ SE ]
    inline constexpr const char* kSeStartButton = "se_title_startbutton.wav";
    inline constexpr const char* kSePlayerShoot = "se_player_shoot.wav";
    inline constexpr const char* kSePlayerDeath = "se_player_death.wav";
    inline constexpr const char* kSePlayerSlowOn = "se_player_slow_on.wav";
    inline constexpr const char* kSePlayerSlowOff = "se_player_slow_off.wav";
    inline constexpr const char* kSeEnemyDeath = "se_enemy_death.wav";
    inline constexpr const char* kSeEnemyRusherAim = "se_enemyrusher_aim.wav";

    namespace UI
    {
        inline constexpr const char* kConfirm = "ui/confirm.wav";
        inline constexpr const char* kHover = "ui/hover.wav";
    }
}

namespace Path::Model
{
    inline constexpr const char* kParticlePlane = "particle/ParticlePlane.obj";
    inline constexpr const char* kPlayer = "Cube/Cube.obj";
    inline constexpr const char* kEnemyNormal = "Cube/Cube.obj";
    inline constexpr const char* kEnemyRusher = "Cube/Cube.obj";
}

namespace Path::ParticleEmitter
{
    inline constexpr const char* kSaveDir = "resources/json/particles/";
    inline constexpr const char* kPlayerConstantTrail = "Player_Constant_Trail.json";
    inline constexpr const char* kGameOverExplosion = "Scene_GameOver_Explosion.json";
    inline constexpr const char* kGameClearExplosion = "Scene_GameClear_Explosion.json";
    inline constexpr const char* kEnemyNormalDeathExplosion = "EnemyNormal_Death_Explosion.json";
    inline constexpr const char* kEnemyNormalDeathSpark = "EnemyNormal_Death_Spark.json";
}
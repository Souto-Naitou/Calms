#pragma once
#include <memory>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <entity/player/Player.h>
#include <Features/GameEye/GameEye.h>
#include <unordered_map>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Lighting/PointLight/PointLight.h>
#include <drawable/sprite/Sprite.h>

class GameClearAnimation
{
public:
    struct Params
    {
        GameEye* pGameEye = nullptr;
        Player* pPlayer = nullptr;
        PointLight* pPointLight = nullptr;
        Particle* pParticle = nullptr;
        Sprite* pSpriteClear = nullptr;
        Sprite* pSpriteSpace = nullptr;
    };

    enum class State
    {
        Start,
        ShakeCamera,
        ApproachCamera,
        ClearSpriteAppear,
        plIntensity,
        End,
    };

    struct Original
    {
        Vector3 playerPosition = {};
        Vector3 playerScale = {};
        Vector3 cameraPosition = {};
        Vector3 cameraRotate = {};
        float pointLightIntensity = 0.0f;
    };

    GameClearAnimation() = default;
    ~GameClearAnimation() = default;
    void Initialize(Params params);
    void Finalize() {};
    void Update();
    void Play();
    void Reset();
    bool IsPlaying() const { return timer_.GetIsStart(); }
    bool IsFinished() const { return isFinished_; }

private:
    void ShakeCameraUpdate();
    void CameraApproach();
    void LightIntensityUpdate();
    void SpriteClearUpdate();

    inline static const std::unordered_map<State, float> stateDurations_ =
    {
        { State::Start,             0.0f },
        { State::ShakeCamera,       0.7f },
        { State::ApproachCamera,    1.2f },
        { State::ClearSpriteAppear, 2.2f },
        { State::plIntensity,       3.0f },
        { State::End,               3.5f },
    };

    Original original_ = {};

    TimeMeasurer timer_ = {};

    std::unique_ptr<ParticleEmitter> pEmitter_ = nullptr;
    Params initParams_ = {};
    float numSpriteColor_ = 0.0f;


    bool isFinished_ = false;
};
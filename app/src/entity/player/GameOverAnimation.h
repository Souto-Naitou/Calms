#pragma once
#include <memory>
#include <drawable/particle/Emitter/ParticleEmitter.h>
#include <entity/player/Player.h>
#include <Features/GameEye/GameEye.h>
#include <unordered_map>
#include <Features/TimeMeasurer/TimeMeasurer.h>
#include <Features/Lighting/PointLight/PointLight.h>

class GameOverAnimation
{
public:
    struct Params
    {
        GameEye* pGameEye = nullptr;
        Player* pPlayer = nullptr;
        PointLight* pPointLight = nullptr;
        Particle* pParticle = nullptr;
    };

    enum class State
    {
        Start,
        ShakeCamera,
        SmallScaling,
        plIntensity,
        dlColor,
        End,
    };

    struct Original
    {
        Vector3 playerScale = {};
        Vector3 cameraPosition = {};
        float pointLightIntensity = 0.0f;
    };

    GameOverAnimation() = default;
    ~GameOverAnimation() = default;
    void Initialize(Params params);
    void Finalize() {};
    void Update();
    void Play();
    void Reset();
    bool IsPlaying() const { return timer_.GetIsStart(); }
    bool IsFinished() const { return isFinished_; }

private:
    void ShakeCameraUpdate();
    void SmallScalation();
    void LightIntensityUpdate();

    inline static const std::unordered_map<State, float> stateDurations_ = 
    {
        { State::Start,          0.0f },
        { State::ShakeCamera,    0.7f },
        { State::SmallScaling,   1.0f },
        { State::plIntensity,    1.2f },
        { State::End,            1.5f },
    };

    Original original_ = {};

    TimeMeasurer timer_ = {};

    std::unique_ptr<ParticleEmitter> pEmitter_ = nullptr;
    Params initParams_ = {};


    bool isFinished_ = false;
};
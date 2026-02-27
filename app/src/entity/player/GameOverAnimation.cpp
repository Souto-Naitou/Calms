#include "GameOverAnimation.h"
#include <mathExtension.h>
#include <Math/Easing.h>
#include <config/ResourcePath.h>


void GameOverAnimation::Initialize(Params params)
{
    initParams_ = params;
    ParticleEmitter::Params emitterParams = {};
    emitterParams.particle = initParams_.pParticle;
    emitterParams.jsonPath = Path::ParticleEmitter::kGameOverExplosion;
    pEmitter_ = std::make_unique<ParticleEmitter>();
    pEmitter_->Initialize(emitterParams);
    pEmitter_->SetEnableBillboard(true);
    pEmitter_->EnableManualMode();
}

void GameOverAnimation::Update()
{
    pEmitter_->SetPosition(initParams_.pPlayer->GetObject3d()->GetTranslate());
    pEmitter_->Update();

    if (!timer_.GetIsStart())
    {
        return;
    }

    ShakeCameraUpdate();
    SmallScalation(); 
    LightIntensityUpdate();

    if (timer_.GetNow<float>() > stateDurations_.at(State::End))
    {
        // アニメーション終了
        isFinished_ = true;
        timer_.Stop();
    }
}

void GameOverAnimation::Play()
{
    timer_.Reset();
    timer_.Start();
    original_.playerScale = initParams_.pPlayer->GetObject3d()->GetScale();
    original_.cameraPosition = initParams_.pGameEye->GetTransform().translate;
    original_.pointLightIntensity = initParams_.pPointLight->GetIntensity();
    pEmitter_->Emit();
    pEmitter_->Emit();
}

void GameOverAnimation::Reset()
{
    timer_.Reset();
    isFinished_ = false;

    // 元の状態に戻す
    auto obj = initParams_.pPlayer->GetObject3d();
    obj->SetScale(original_.playerScale);
    initParams_.pGameEye->SetTranslate(original_.cameraPosition);
    initParams_.pPointLight->GetIntensity() = original_.pointLightIntensity;
}

void GameOverAnimation::ShakeCameraUpdate()
{
    if (timer_.GetNow<float>() > stateDurations_.at(State::ShakeCamera))
    {
        // 時間経過したら終了
        return;
    }

    // カメラシェイク処理
    float t = stateDurations_.at(State::ShakeCamera) - timer_.GetNow<float>();
    t = t / stateDurations_.at(State::ShakeCamera);
    initParams_.pGameEye->Shake(t * 0.5f);
}

void GameOverAnimation::SmallScalation()
{
    if (timer_.GetNow<float>() > stateDurations_.at(State::SmallScaling))
    {
        // 時間経過したら終了
        auto obj = initParams_.pPlayer->GetObject3d();
        obj->SetScale({});
        return;
    }

    float t = timer_.GetNow<float>() / stateDurations_.at(State::SmallScaling);
    float easedT = Math::Easing::EaseInOutQuad(t);

    auto obj = initParams_.pPlayer->GetObject3d();
    obj->SetScale(Math::Lerp(original_.playerScale, {}, easedT));
}

void GameOverAnimation::LightIntensityUpdate()
{
    if (timer_.GetNow<float>() > stateDurations_.at(State::SmallScaling))
    {
        // 時間経過したら終了
        initParams_.pPointLight->GetIntensity() = 0.0f;
        return;
    }

    float t = timer_.GetNow<float>() / stateDurations_.at(State::SmallScaling);
    float easedT = Math::Easing::EaseInOutQuad(t);
    initParams_.pPointLight->GetIntensity() = std::lerp(original_.pointLightIntensity, 0.0f, easedT);
}

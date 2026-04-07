#include "GameClearAnimation.h"
#include <mathExtension.h>
#include <Math/Easing.h>
#include <config/ResourcePath.h>
#include <Core/DirectX12/TextureManager.h>
#include <Core/Window/Window.h>
#include <cmath>
#include <Color.h>


void GameClearAnimation::Initialize(Params params)
{
    initParams_ = params;
    ParticleEmitter::Params emitterParams = {};
    emitterParams.particle = initParams_.pParticle;
    emitterParams.jsonPath = Path::ParticleEmitter::kGameClearExplosion;
    pEmitter_ = std::make_unique<ParticleEmitter>();
    pEmitter_->Initialize(emitterParams);
    pEmitter_->SetEnableBillboard(true);
    pEmitter_->EnableManualMode();

    std::array<D3D12_GPU_DESCRIPTOR_HANDLE, 10> textureHandles = {};
    for (uint32_t i = 0; i < 10; ++i)
    {
        std::string texturePath = Path::Image::kNumbers[i];
        textureHandles[i] = TextureManager::GetInstance()->GetSrvHandleGPU(texturePath);
    }
    pScore_ = std::make_unique<NumericView>();
    pScore_->Initialize(textureHandles);
    pScore_->SetFontSize(56.0f);
    pScore_->SetColor(RGBA(0xc5a44600).to_Vector4());
    auto& layoutProp = pScore_->GetFontLayoutProperties();
    layoutProp.leftTop = { static_cast<float>(Window::clientWidth) / 4.0f, static_cast<float>(Window::clientHeight) / 2.0f };
    layoutProp.anchorPoint = { 0.5f, 0.5f };
    layoutProp.letterSpacing = 32.0f;

    pInputAwareSprite_ = std::make_unique<InputAwareSprite>();
    InputAwareSprite::Entry entry = {};
    entry.pSprite_ = initParams_.pSpriteSpace;
    entry.handleKeyboard_= TextureManager::GetInstance()->GetSrvHandleGPU(Path::Image::kTitleStartPromptSpaceKey);
    entry.handleGamepad_  = TextureManager::GetInstance()->GetSrvHandleGPU(Path::Image::kTitleStartPromptButtonA);
    pInputAwareSprite_->Initialize();
    pInputAwareSprite_->AddEntry(entry);
}

void GameClearAnimation::Update()
{
    pEmitter_->SetPosition(initParams_.pPlayer->GetObject3d()->GetTranslate());
    pEmitter_->Update();

    pScore_->SetNumber(static_cast<uint32_t>(original_.score));
    pScore_->Update();

    if (!timer_.GetIsStart())
    {
        return;
    }

    ShakeCameraUpdate();
    CameraApproach();
    SpriteClearUpdate();
    LightIntensityUpdate();

    if (timer_.GetNow<float>() > stateDurations_.at(State::End))
    {
        // アニメーション終了
        isFinished_ = true;
        timer_.Stop();
    }
}

void GameClearAnimation::Draw1F()
{
    pScore_->Draw1F();
}

void GameClearAnimation::Play()
{
    timer_.Reset();
    timer_.Start();
    original_.playerScale = initParams_.pPlayer->GetObject3d()->GetScale();
    original_.cameraPosition = initParams_.pGameEye->GetTransform().translate;
    original_.pointLightIntensity = initParams_.pPointLight->GetData().intensity;
    original_.playerPosition = initParams_.pPlayer->GetObject3d()->GetTranslate();
    original_.cameraRotate = initParams_.pGameEye->GetTransform().rotate;
    original_.score = initParams_.pScoreCalculator->GetScore();
    pEmitter_->Emit();
    pEmitter_->Emit();
}

void GameClearAnimation::Reset()
{
    timer_.Reset();
    isFinished_ = false;

    // 元の状態に戻す
    auto obj = initParams_.pPlayer->GetObject3d();
    obj->SetScale(original_.playerScale);
    initParams_.pGameEye->SetTranslate(original_.cameraPosition);
    initParams_.pPointLight->GetData().intensity = original_.pointLightIntensity;
}

void GameClearAnimation::ShakeCameraUpdate()
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

void GameClearAnimation::CameraApproach()
{
    if (timer_.GetNow<float>() > stateDurations_.at(State::ApproachCamera))
    {
        // 時間経過したら終了
        return;
    }

    // カメラ接近処理
    float t = timer_.GetNow<float>() / stateDurations_.at(State::ApproachCamera);
    float easedT = Math::Easing::EaseInOutCubic(t);
    Vector3 targetPosition = original_.playerPosition + Vector3(-4.0f, 5.0f, -4.0f);
    Vector3 targetRotate = Vector3(0.74f, 0.62f, 0.0f);
    initParams_.pGameEye->SetRotate(Math::Lerp(original_.cameraRotate, targetRotate, easedT));
    initParams_.pGameEye->SetTranslate(Math::Lerp(original_.cameraPosition, targetPosition, easedT));
}

void GameClearAnimation::LightIntensityUpdate()
{
    auto& data = initParams_.pPointLight->GetData();

    if (timer_.GetNow<float>() > stateDurations_.at(State::plIntensity))
    {
        // 時間経過したら終了
        data.intensity= 5.0f;
        return;
    }

    float t = timer_.GetNow<float>() / stateDurations_.at(State::plIntensity);
    float easedT = Math::Easing::EaseInQuad(t);
    data.intensity = std::lerp(original_.pointLightIntensity, 5.0f, easedT);
}

void GameClearAnimation::SpriteClearUpdate()
{
    if (timer_.GetNow<float>() <= stateDurations_.at(State::ApproachCamera))
    {
        // 時間経過してなかったら飛ばす
        return;
    }
    if (timer_.GetNow<float>() > stateDurations_.at(State::ClearSpriteAppear))
    {
        // 時間経過したら終了
        numSpriteColor_ += 0.05f;
        // 0.5f 〜 1.0f の範囲で明滅
        float alpha = (std::sin(numSpriteColor_) + 1.0f) / 4.0f + 0.5f;
        initParams_.pSpriteClear->SetColor({ 1.0f, 1.0f, 1.0f, alpha });
        initParams_.pSpriteSpace->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        return;
    }

    float t = (timer_.GetNow<float>() - stateDurations_.at(State::ApproachCamera))
        / (stateDurations_.at(State::ClearSpriteAppear) - stateDurations_.at(State::ApproachCamera));
    float easedT = Math::Easing::EaseInOutCubic(t);
    initParams_.pSpriteClear->SetColor(Vector4(1.0f, 1.0f, 1.0f, easedT));
    initParams_.pSpriteSpace->SetColor(Vector4(1.0f, 1.0f, 1.0f, easedT));

    Vector4 scoreColor = pScore_->GetColor();
    pScore_->SetColor(Vector4(scoreColor.xyz(), easedT));
}

#include "PlayerUI3d.h"
#include "nima_engine/modules/vectormatrix/math/Vector4.h"
#include <config/ResourcePath.h>



void PlayerUI3d::Initialize(DirectX12* pDx12)
{
    this->InitializeGaugeExplosion(pDx12);
    this->InitializeGaugeHP(pDx12);
    this->InitializeGaugeSlomo(pDx12);
}

void PlayerUI3d::Finalize()
{
    pGaugeSlomo_->Finalize();
    pGaugeExplosion_->Finalize();
    pGaugeHP_->Finalize();
}

void PlayerUI3d::Update(const Params& param)
{
    // 爆発ゲージ更新
    const float explosionRatio = param.explosionScore / param.explosionScoreMax;
    *pGaugeExplosion_ = explosionRatio;
    pGaugeExplosion_->SetPosition(center_);
    pGaugeExplosion_->Update();
    // HPゲージ更新
    const float hpRatio = param.hp / param.hpMax;
    *pGaugeHP_ = hpRatio;
    pGaugeHP_->SetPosition(center_);
    pGaugeHP_->Update();
    // スロモゲージ更新
    const float slomoTimeRatio = param.slomoTime / param.slomoTimeMax;
    *pGaugeSlomo_ = slomoTimeRatio;
    pGaugeSlomo_->SetPosition(center_);
    pGaugeSlomo_->Update();
}

void PlayerUI3d::Draw1F()
{
    pGaugeExplosion_->Draw1F();
    pGaugeHP_->Draw1F();
    pGaugeSlomo_->Draw1F();
}

void PlayerUI3d::ImGui()
{

}

void PlayerUI3d::InitializeGaugeExplosion(DirectX12* pDx12)
{
    RingModel::Params modelParams = {};
    modelParams.pDx12           = pDx12;
    modelParams.radiusInner     = kGaugeExplosionInnerRadius_;
    modelParams.radiusOuter     = kGaugeExplosionInnerRadius_ + kGaugeThickness_;
    modelParams.textureFilePath = Path::Image::kWhite;
    modelParams.angleOffsetRad  = std::numbers::pi_v<float> * 2.0f * 0.25f; // 1/4
    RingGauge::Params params;
    params.backgroundParams = modelParams;
    params.fillParams       = modelParams;
    params.colorBackground  = 0x404040ff;
    params.colorFill        = 0xffffffff;
    params.colorTarget      = 0xff4040ff;
    params.lerpFactor       = 0.3f;
    params.valueInit        = 0.0f;
    params.isFlipRing       = true;
    pGaugeExplosion_ = std::make_unique<RingGauge>();
    pGaugeExplosion_->Initialize(params);
}

void PlayerUI3d::InitializeGaugeHP(DirectX12* pDx12)
{
    RingModel::Params modelParams = {};
    modelParams.pDx12           = pDx12;
    modelParams.radiusInner     = kGaugeHPInnerRadius_;
    modelParams.radiusOuter     = kGaugeHPInnerRadius_ + kGaugeThickness_;
    modelParams.angelEndRad     = std::numbers::pi_v<float> * 2.0f * 0.25f; // 1/4
    modelParams.angleOffsetRad  = std::numbers::pi_v<float> * 2.0f * 0.125f; // 1/8
    modelParams.textureFilePath = Path::Image::kWhite;
    RingGauge::Params params;
    params.backgroundParams = modelParams;
    params.fillParams       = modelParams;
    params.colorBackground  = 0x404040ff;
    params.colorFill        = 0x479f62ff;
    params.lerpFactor       = 0.2f;
    params.valueInit        = 1.0f;
    pGaugeHP_ = std::make_unique<RingGauge>();
    pGaugeHP_->Initialize(params);
}

void PlayerUI3d::InitializeGaugeSlomo(DirectX12* pDx12)
{
    RingModel::Params modelParams = {};
    modelParams.pDx12           = pDx12;
    modelParams.radiusOuter     = kGaugeSlomoInnerRadius_ + kGaugeThickness_;
    modelParams.radiusInner     = kGaugeSlomoInnerRadius_;
    modelParams.angelEndRad     = std::numbers::pi_v<float> * 2.0f * 0.25f; // 1/4
    modelParams.angleOffsetRad  = std::numbers::pi_v<float> * 5.0f / 4.0f; // 1/4_pi
    modelParams.textureFilePath = Path::Image::kWhite;
    RingGauge::Params params;
    params.backgroundParams = modelParams;
    params.fillParams = modelParams;
    params.colorBackground = 0x404040ff;
    params.colorFill = 0x93dbfbff;
    params.lerpFactor = 1.0f;
    params.valueInit = 1.0f;
    pGaugeSlomo_ = std::make_unique<RingGauge>();
    pGaugeSlomo_->Initialize(params);
}

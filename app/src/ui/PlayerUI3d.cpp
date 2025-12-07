#include "PlayerUI3d.h"
#include "nima_engine/modules/vectormatrix/math/Vector4.h"
#include <config/ResourcePath.h>



void PlayerUI3d::Initialize(Player* player, DirectX12* pDx12)
{
    pContext_ = &player->GetContext();
    pStats_ = static_cast<const EntityStats*>(player->GetStats());
    this->InitializeGaugeExplosion(pDx12);
    this->InitializeGaugeHP(pDx12);
}

void PlayerUI3d::Finalize()
{
    gaugeExplosion_->Finalize();
    gaugeHP_->Finalize();
}

void PlayerUI3d::Update()
{
    // 爆発ゲージ更新
    const float explosionScore = pContext_->Get().explosionScore;
    const float explosionRatio = explosionScore / PlayerContext::kMaxExplosionScore;
    *gaugeExplosion_ = explosionRatio;
    gaugeExplosion_->SetPosition(position_);
    gaugeExplosion_->Update();
    // HPゲージ更新
    const float hp = pStats_->GetHp();
    const float maxHp = pStats_->GetMaxHp();
    const float hpRatio = hp / maxHp;
    *gaugeHP_ = hpRatio;
    gaugeHP_->SetPosition(position_);
    gaugeHP_->Update();
}

void PlayerUI3d::Draw1F()
{
    gaugeExplosion_->Draw1F();
    gaugeHP_->Draw1F();
}

void PlayerUI3d::ImGui()
{

}

void PlayerUI3d::InitializeGaugeExplosion(DirectX12* pDx12)
{
    RingModel::Params modelParams = {};
    modelParams.pDx12 = pDx12;
    modelParams.radiusOuter = 4.1f;
    modelParams.radiusInner = 4.0f;
    modelParams.textureFilePath = Path::Image::kWhite;
    modelParams.radAngleOffset = std::numbers::pi_v<float> * 2.0f * 0.25f; // 1/4
    RingGauge::Params params;
    params.backgroundParams = modelParams;
    params.fillParams = modelParams;
    params.colorBackground = 0x404040ff;
    params.colorFill = 0xffffffff;
    params.colorTarget = 0xff4040ff;
    params.lerpFactor = 0.3f;
    params.valueInit = 0.0f;
    params.isFlipRing = true;
    gaugeExplosion_ = std::make_unique<RingGauge>();
    gaugeExplosion_->Initialize(params);
}

void PlayerUI3d::InitializeGaugeHP(DirectX12* pDx12)
{
    RingModel::Params modelParams = {};
    modelParams.pDx12 = pDx12;
    modelParams.radiusOuter = 3.6f;
    modelParams.radiusInner = 3.4f;
    modelParams.radAngleEnd = std::numbers::pi_v<float> * 2.0f * 0.25f; // 1/4
    modelParams.radAngleOffset = std::numbers::pi_v<float> * 2.0f * 0.125f; // 1/8
    modelParams.textureFilePath = Path::Image::kWhite;
    RingGauge::Params params;
    params.backgroundParams = modelParams;
    params.fillParams = modelParams;
    params.colorBackground = 0x404040ff;
    params.colorFill = 0x479f62ff;
    params.lerpFactor = 0.2f;
    params.valueInit = 1.0f;
    gaugeHP_ = std::make_unique<RingGauge>();
    gaugeHP_->Initialize(params);
}

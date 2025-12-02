#include "RingGauge.h"
#include <cmath>
#include <algorithm>



void RingGauge::Initialize(const Params& params)
{
    pDebugEntry_ = std::make_unique<DebugEntry<RingGauge>>("UI", "RingGauge", this);
    params_ = params;
    currentValue_ = params_.valueInit;
    this->InitializeModels();
    this->InitializeObjects();
}

void RingGauge::Finalize()
{
    objectRingBackground_->Finalize();
    objectRingFill_->Finalize();
}

void RingGauge::Update()
{
    currentValue_ = std::clamp(currentValue_, 0.0f, 1.0f);
    const float angleStart = params_.fillParams.radAngleStart;
    const float angleEnd = params_.fillParams.radAngleEnd;
    const float angleEndNew = std::lerp(angleStart, angleEnd, currentValue_);

    modelRingFill_->SetAngleRange(Range(angleStart, angleEndNew));

    objectRingBackground_->Update();
    objectRingFill_->Update();
}

void RingGauge::Draw1F()
{
    objectRingBackground_->Draw1F();
    objectRingFill_->Draw1F();
}

void RingGauge::ImGui()
{
    ImGui::DragFloat("Current Value", & currentValue_, 0.01f, 0.0f, 1.0f);
    float angleStart = params_.backgroundParams.radAngleStart;
    if (ImGui::SliderAngle("StartAngle", &angleStart, 0.01f))
    {
        params_.backgroundParams.radAngleStart = angleStart;
        params_.fillParams.radAngleStart = angleStart;
        modelRingBackground_->SetAngleRange(Range<float>(angleStart, params_.backgroundParams.radAngleEnd));
        modelRingFill_->SetAngleRange(Range<float>(angleStart, params_.fillParams.radAngleEnd));
    }
    float angleEnd = params_.backgroundParams.radAngleEnd;
    if (ImGui::SliderAngle("EndAngle", &angleEnd, 0.01f))
    {
        params_.backgroundParams.radAngleEnd = angleEnd;
        params_.fillParams.radAngleEnd = angleEnd;
        modelRingBackground_->SetAngleRange(Range<float>(params_.backgroundParams.radAngleStart, angleEnd));
        modelRingFill_->SetAngleRange(Range<float>(params_.fillParams.radAngleStart, angleEnd));
    }
}

void RingGauge::SetPosition(const Vector3& pos)
{
    position_ = pos;
    objectRingBackground_->SetTranslate(position_);
    position_.y += 0.01f;
    objectRingFill_->SetTranslate(position_);
}

void RingGauge::InitializeModels()
{
    modelRingBackground_ = std::make_unique<RingModel>(params_.backgroundParams);
    modelRingBackground_->Initialize();
    modelRingFill_ = std::make_unique<RingModel>(params_.fillParams);
    modelRingFill_->Initialize();
}

void RingGauge::InitializeObjects()
{
    objectRingBackground_ = std::make_unique<Object3d>();
    objectRingBackground_->Initialize();
    objectRingBackground_->SetModel(modelRingBackground_.get());
    {
        auto& option = objectRingBackground_->GetOption();
        option.materialData->color = params_.colorBackground.to_Vector4();
        option.lightingData->enableLighting = false;
        option.materialData->environmentCoefficient = 0.0f;
    }

    objectRingFill_ = std::make_unique<Object3d>();
    objectRingFill_->Initialize();
    objectRingFill_->SetModel(modelRingFill_.get());
    {
        auto& option = objectRingFill_->GetOption();
        option.materialData->color = params_.colorFill.to_Vector4();
        option.lightingData->enableLighting = false;
        option.materialData->environmentCoefficient = 0.0f;
    }
}

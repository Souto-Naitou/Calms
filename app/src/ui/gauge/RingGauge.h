#pragma once
#include <memory>
#include <Features/Primitive/RingModel.h>
#include <drawable/object3d/Object3d.h>
#include <Color.h>
#include <Features/Gauge/GaugeBase.h>
#include <Vector3.h>
#include <DebugTools/DebugEntry/DebugEntry.h>
#include <optional>

class RingGauge : public GaugeBase
{
public:
    using GaugeBase::operator=;

    struct Params
    {
        RingModel::Params   backgroundParams;
        RingModel::Params   fillParams;
        bool                isFlipRing          = false;            // リングを反転させるかどうか
        RGBA                colorBackground     = 0xffffffff;
        RGBA                colorFill           = 0xffffffff;
        std::optional<RGBA> colorTarget         = std::nullopt;
        float               lerpFactor          = 1.0f; // 0.0f ~ 1.0f (1.0fは即時)
        float               valueInit           = 1.0f; // 0.0f ~ 1.0f
    };

    RingGauge() = default;
    ~RingGauge() = default;

    void Initialize(const Params& params);
    void Finalize();
    void Update();
    void Draw1F();
    void ImGui();
    void SetPosition(const Vector3& pos);
    void SetName(const std::string& name);

private:
    void InitializeModels();
    void InitializeObjects();

    Params params_;
    std::unique_ptr<DebugEntry<RingGauge>> pDebugEntry_ = nullptr;
    std::unique_ptr<RingModel> modelRingBackground_;
    std::unique_ptr<RingModel> modelRingFill_;
    std::unique_ptr<Object3d>  objectRingBackground_;
    std::unique_ptr<Object3d>  objectRingFill_;
    Vector3                    position_ = {};
};
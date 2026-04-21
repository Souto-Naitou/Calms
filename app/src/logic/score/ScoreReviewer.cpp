#include "ScoreReviewer.h"
#include <config/ResourcePath.h>
#include <utility/JSONIO/JSONIO.h>

void ScoreReviewer::Initialize()
{
    pDebugEntry_ = std::make_unique<DebugEntry<ScoreReviewer>>("ScoreReviewer", this, false);

    thresholds_ = {
        {std::to_string(static_cast<int>(Result::S)), 90.0f},
        {std::to_string(static_cast<int>(Result::A)), 80.0f},
        {std::to_string(static_cast<int>(Result::B)), 70.0f},
        {std::to_string(static_cast<int>(Result::C)), 0.0f}
    };

    this->LoadFromJSON();
}

ScoreReviewer::Result ScoreReviewer::Review(float score)
{
    Result result = Result::C;

    for (const auto& [key, threshold] : thresholds_.items())
    {
        if (score >= threshold)
        {
            result = static_cast<Result>(std::stoi(key));
            break;
        }
    }

    return result;
}

void ScoreReviewer::ImGui()
{
    #ifdef _DEBUG

    for (const auto& [key, threshold] : thresholds_.items())
    {
        float value = threshold.get<float>();
        ImGui::InputFloat(("Threshold " + key).c_str(), &value);
        thresholds_[key] = value;
    }

    if (ImGui::Button("Save"))
    {
        this->SaveToJSON();
    }

    #endif // _DEBUG
}

void ScoreReviewer::SaveToJSON() const
{
    std::string path = std::string(Path::Resource::kJsonDir) + Path::Json::kScoreReviewerThresholds;
    JSONIO::GetInstance()->Save(path, thresholds_);
}

void ScoreReviewer::LoadFromJSON()
{
    std::string path = std::string(Path::Resource::kJsonDir) + Path::Json::kScoreReviewerThresholds;
    thresholds_ = JSONIO::GetInstance()->Load(path);
}

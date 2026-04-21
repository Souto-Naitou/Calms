#pragma once

#include <nlohmann/json.hpp>
#include <DebugTools/DebugEntry/DebugEntry.h>

class ScoreReviewer
{
public:
    enum class Result
    {
        S,
        A,
        B,
        C,
        SIZE
    };
    void Initialize();
    Result Review(float score);
    void ImGui();

private:
    void SaveToJSON() const;
    void LoadFromJSON();

    using json = nlohmann::json;
    json thresholds_;
    std::unique_ptr<DebugEntry<ScoreReviewer>> pDebugEntry_;
};
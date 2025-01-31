#pragma once
#include <Features/Text/Text.h>
#include <memory>

class ScoreSystem
{
public:
    void Initialize();
    void Update();
    void DrawTxt();
    void Finalize();

    void CountEnemyDeath();

private:
    static const unsigned int kEnemyScore = 100;

    float score_;
    unsigned int enemyDeathCount_;
    float receiveAddScore_;

    unsigned int framesAddScore_ = 20u;
    std::unique_ptr<Text> pName_ = nullptr;
    std::unique_ptr<Text> pScore_ = nullptr;
};
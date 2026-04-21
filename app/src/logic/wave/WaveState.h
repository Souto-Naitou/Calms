#pragma once

struct WaveState
{
    bool isWaveActive = false;  // !< ウェーブがアクティブかどうか
    int currentWaveIndex = 0;   // !< 現在のウェーブのインデックス
    int currentBudget = 0;      // !< 現在のウェーブの予算
    float elapsedTime = 0.0f;   // !< 現在のウェーブの経過時間
};
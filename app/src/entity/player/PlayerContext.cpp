#include "PlayerContext.h"



void PlayerContext::IncreaseExplosionScore(float amount)
{
    status_.explosionScore += amount;
    if (status_.explosionScore > kMaxExplosionScore)
    {
        status_.explosionScore = kMaxExplosionScore;
    }
}

void PlayerContext::DecreaseExplosionScore(float amount)
{
    status_.explosionScore -= amount;
    if (status_.explosionScore < 0.0f)
    {
        status_.explosionScore = 0.0f;
    }
}

#pragma once
#include <presentation/ParticleType.h>
#include <Vector3.h>

struct ParticleEmitEvent
{
    ParticleType type = {};
    Vector3 position = {};
};
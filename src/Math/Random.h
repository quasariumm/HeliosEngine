#pragma once

#include <cstdint>

#include "Vector.h"


namespace Engine
{

extern uint32_t RandomSeed(uint32_t val);

extern uint32_t RandomUInt(uint32_t& seed, uint32_t min = 0, uint32_t max = UINT_MAX);

extern float RandomFloat(uint32_t& seed, float min = 0.f, float max = 1.f);

extern vec3f RandomDirection(uint32_t& seed);

extern vec2f RandomPointInUnitCircle(uint32_t& seed);

};
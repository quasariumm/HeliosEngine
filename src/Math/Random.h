#pragma once

#include <cstdint>

#include "Vector.h"


namespace Engine
{

static uint32_t RandomSeed(uint32_t val);

static uint32_t RandomUInt(uint32_t& seed, uint32_t min = 0, uint32_t max = UINT_MAX);

static float RandomFloat(uint32_t& seed, float min = 0.f, float max = 1.f);

static vec3f RandomDirection(uint32_t& seed);

static vec2f RandomPointInUnitCircle(uint32_t& seed);

};
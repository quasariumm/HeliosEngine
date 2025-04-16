#include "Random.h"


namespace Engine
{

uint32_t RandomSeed(const uint32_t val)
{
	uint32_t s = ((time(nullptr) >> 3) ^ 0x2167351) * val % 12579;

	s = (s ^ 61) ^ (s >> 16);
	s *= 9, s = s ^ (s >> 4);
	s *= 0x27d4eb2d;
	s = s ^ (s >> 15);
	return s;
}


uint32_t RandomUInt(uint32_t& seed, const uint32_t min, const uint32_t max)
{
	seed = seed * 747796405u + 2891336453u;
	seed = ((seed >> ((seed >> 28u) + 4u)) ^ seed) * 277803737u;
	seed = (seed >> 22u) ^ seed;
	return min + seed % (max - min);
}


float RandomFloat(uint32_t& seed, const float min, const float max)
{
	const float result = static_cast<float>(RandomUInt(seed)) / 4294967295.f;
	return min + (max - min) * result;
}


vec3f RandomDirection(uint32_t& seed)
{
	return Normalise( vec3f(RandomFloat(seed, -1.f, 1.f), RandomFloat(seed, -1.f, 1.f), RandomFloat(seed, -1.f, 1.f)) );
}


vec2f RandomPointInUnitCircle(uint32_t& seed)
{
	float x, y;
	do
	{
		x = 2.f * RandomFloat(seed) - 1.f;
		y = 2.f * RandomFloat(seed) - 1.f;
	}
	while (x * x + y * y > 1.f);
	return { x, y };
}

}

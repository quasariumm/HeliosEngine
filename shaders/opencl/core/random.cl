#ifndef RANDOM_CL_
#define RANDOM_CL_

uint Randomise(uint* seedOut)
{
    uint seed = *seedOut;
    seed = seed * 747796405u + 2891336453u;
	seed = ((seed >> ((seed >> 28u) + 4u)) ^ seed) * 277803737u;
	seed = (seed >> 22u) ^ seed;
    *seedOut = seed;
	return seed;
}

uint WangHash(uint* outSeed)
{
    uint seed = *outSeed;
	// Wang hash initialisation
	seed = (seed ^ 61u) ^ (seed >> 16u);
	seed *= 9u, seed = seed ^ (seed >> 4u);
	seed *= 0x27d4eb2du;
	seed = seed ^ (seed >> 15u);
    *outSeed = seed;
	return seed;
}

float Xi(uint* seed)
{
    return Randomise(seed) / 4294967295.f;
}

float RandomFloat(uint* seed)
{
	return Randomise(seed) / 4294967295.f;
}

// Random value in normal distribution (with mean=0 and sd=1)
float RandomValueNormalDistribution(uint* seed)
{
	// Thanks to https://stackoverflow.com/a/6178290
	float theta = M_PI_2_F * RandomFloat(seed);
	float rho = sqrt(-2.f * log(RandomFloat(seed)));
	return rho * cos(theta);
}

// Calculate a random direction
float3 RandomHemisphericalDirection(uint* seed)
{
	// Thanks to https://math.stackexchange.com/a/1585996
	float x = RandomValueNormalDistribution(seed);
	float y = RandomValueNormalDistribution(seed);
	float z = RandomValueNormalDistribution(seed);
	if (z < 0)
	    return normalize((float3)(-x, -y, -z));
	return normalize((float3)(x, y, z));
}


float2 RandomPointInCircle(uint* seed)
{
	float angle = RandomFloat(seed) * M_PI_2_F;
	float2 pointOnCircle = (float2)(cos(angle), sin(angle));
	return pointOnCircle * sqrt(RandomFloat(seed));
}

#endif
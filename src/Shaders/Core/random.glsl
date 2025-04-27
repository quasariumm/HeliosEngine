#ifndef RANDOM_GLSL
#define RANDOM_GLSL

/*
	Random
	DO NOT use this as a standalone shader.
	This is meant to be included in another shader file
*/

uint Randomise(inout uint seed)
{
	seed = seed * 747796405u + 2891336453u;
	seed = ((seed >> ((seed >> 28u) + 4u)) ^ seed) * 277803737u;
	seed = (seed >> 22u) ^ seed;
	return seed;
}

float RandomFloat(inout uint seed)
{
	return Randomise(seed) / 4294967295.f;
}

// Random value in normal distribution (with mean=0 and sd=1)
float RandomValueNormalDistribution(inout uint seed)
{
	// Thanks to https://stackoverflow.com/a/6178290
	float theta = TWO_PI * RandomFloat(seed);
	float rho = sqrt(-2 * log(RandomFloat(seed)));
	return rho * cos(theta);
}


// Calculate a random direction
vec3 RandomHemisphericalDirection(inout uint seed)
{
	// Thanks to https://math.stackexchange.com/a/1585996
	float x = RandomValueNormalDistribution(seed);
	float y = RandomValueNormalDistribution(seed);
	float z = RandomValueNormalDistribution(seed);
	if (z < 0)
	return normalize(vec3(-x, -y, -z));
	return normalize(vec3(x, y, z));
}


vec2 RandomPointInCircle(inout uint seed)
{
	float angle = RandomFloat(seed) * TWO_PI;
	vec2 pointOnCircle = vec2(cos(angle), sin(angle));
	return pointOnCircle * sqrt(RandomFloat(seed));
}

#endif // RANDOM_GLSL
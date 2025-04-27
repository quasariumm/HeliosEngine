#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

/*
	Stores the light classes and light sampling logic
*/

struct DirectionalLight
{
	vec3 color;
	vec3 direction;
	float intensity;
};

vec3 SampleDirectionalLight(DirectionalLight light, vec3 intersection, vec3 normal)
{
	float overlap = dot(-light.direction, normal);
	if (overlap < 0.0)
	{
		return vec3(0.0);
	}
	return light.color * light.intensity * overlap;
}

const DirectionalLight sun = DirectionalLight(vec3(0.9), normalize(vec3(1.0, -1.0, 0.0)), 1.0);

#endif // LIGHTING_GLSL
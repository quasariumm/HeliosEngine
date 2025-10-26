#ifndef LIGHTING_GLSL
#define LIGHTING_GLSL

/*
	Stores the light classes and light sampling logic
*/

/*
	Directional light
*/

struct DirectionalLight
{
	vec3 color;
	float intensity;
	vec3 direction;
	float padding;
};

vec3 SampleDirectionalLight(DirectionalLight light, vec3 intersection, vec3 normal)
{
	float overlap = dot(-light.direction, normal);
	if (overlap < 0.0)
		return vec3(0.0);
	return light.color * light.intensity * overlap;
}

uniform uint NumDirectionalLights = 0;
layout (std430, binding = 0) readonly buffer DirectionalLightBuffer
{
	DirectionalLight DirectionalLights[];
};

/*
	PointLight
*/

struct PointLight
{
	vec3 color;
	float intensity;
	vec3 position;
	float padding;
};

uniform uint NumPointLights = 0;
layout (std430, binding = 1) readonly buffer PointLightBuffer
{
	PointLight PointLights[];
};

vec3 SamplePointLight(PointLight light, vec3 intersection, vec3 normal)
{
	float diff = length(light.position - intersection);
	float overlap = dot(normal, (light.position - intersection) / diff);
	if (overlap < 0.0)
		return vec3(0.0);
	return light.intensity * light.color * (1.0 / (diff * diff)) * overlap;
}

/*
	Simple SpotLight
*/

struct SpotLight
{
	vec3 color;
	float intensity;
	vec3 position;
	float innerCutoff;
	vec3 direction;
	float outerCutoff;
};

uniform uint NumSpotLights = 0;
layout (std430, binding = 2) readonly buffer SpotLightBuffer
{
	SpotLight SpotLights[];
};

vec3 SampleSpotLight(SpotLight light, vec3 intersection, vec3 normal)
{
	vec3 lightDir = normalize(light.position - intersection);
	float theta = dot(lightDir, -light.direction);

	if (theta < 0.0)
		return vec3(0.0);

	float epsilon = (light.innerCutoff - light.outerCutoff);
	float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

	return light.intensity * intensity * light.color * dot(normal, lightDir);
}


// Returns lightVector in first column, light in second
mat2x3 SampleSceneLights(inout uint seed, vec3 intersection, vec3 normal)
{
	uint numLights = NumDirectionalLights + NumPointLights + NumSpotLights;

	if (numLights == 0)
		return mat2x3(vec3(1.0, 0.0, 0.0), vec3(0.0));

	uint sampledLight = uint(floor(Xi(seed) * numLights));
	if (sampledLight < NumDirectionalLights)
	{
		DirectionalLight light = DirectionalLights[sampledLight];
		return mat2x3(normalize(-light.direction), float(numLights) * SampleDirectionalLight(light, intersection, normal));
	}
	else if (sampledLight < NumDirectionalLights + NumPointLights)
	{
		PointLight light = PointLights[sampledLight - NumDirectionalLights];
		return mat2x3(normalize(light.position - intersection), float(numLights) * SamplePointLight(light, intersection, normal));
	}
	else if (sampledLight < NumDirectionalLights + NumPointLights + NumSpotLights)
	{
		// TODO: Fix this not compiling
//		SpotLight light = SpotLights[sampledLight - NumDirectionalLights - NumPointLights];
//		return mat2x3(normalize(light.position - intersection), float(numLights) * SampleSpotLight(light, intersection, normal));
		return mat2x3(0.0);
	}
	else
	{
		return mat2x3(0.0);
	}
}

#endif // LIGHTING_GLSL
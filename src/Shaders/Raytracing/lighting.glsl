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

#define MAX_DIR_LIGHTS 32
uniform DirectionalLight DirectionalLights[MAX_DIR_LIGHTS];
uniform uint NumDirectionalLights = 0;
const DirectionalLight sun = DirectionalLight(vec3(0.9), normalize(vec3(1.0, -1.0, 0.0)), 1.0);

/*
	PointLight
*/

struct PointLight
{
	vec3 position;
	vec3 color;
	float intensity;
};

#define MAX_PNT_LIGHTS 32
uniform PointLight PointLights[MAX_PNT_LIGHTS];
uniform uint NumPointLights = 0;

vec3 SamplePointLight(PointLight light, vec3 intersection, vec3 normal)
{
	float diff = length(light.position - intersection);
	float overlap = dot(normal, (light.position - intersection) / diff);
	return light.intensity * light.color * (1.0 / (diff * diff)) * overlap;
}

/*
	Simple SpotLight
*/

struct SimpleSpotLight
{
	vec3 position;
	vec3 color;
	vec3 direction;
	float intensity;
	float innerCutOff;
	float outerCutOff;
};

#define MAX_SSPT_LIGHTS 32
uniform SimpleSpotLight SimpleSpotLights[MAX_SSPT_LIGHTS];
uniform uint NumSimpleSpotLights = 0;

vec3 SampleSimpleSpotLight(SimpleSpotLight light, vec3 intersection, vec3 normal)
{
	vec3 lightDir = normalize(light.position - intersection);
	float theta = dot(lightDir, -light.direction);
	float epsilon = (light.innerCutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

	return light.intensity * intensity * light.color * dot(normal, lightDir);
}


vec3 SampleSceneLights(inout uint seed, vec3 intersection, vec3 normal)
{
	uint numLights = NumDirectionalLights + NumPointLights + NumSimpleSpotLights;
	float chance = 1.0 / numLights;

	uint sampledLight = uint(round(Xi(seed) * numLights));
	if (sampledLight < NumDirectionalLights)
	{
		return SampleDirectionalLight(
			DirectionalLights[sampledLight],
			intersection,
			normal
		);
	}
	else if (sampledLight < NumDirectionalLights + NumPointLights)
	{
		return SamplePointLight(
			PointLights[sampledLight - NumDirectionalLights],
			intersection,
			normal
		);
	}
	else
	{
		return SampleSimpleSpotLight(
			SimpleSpotLights[sampledLight - NumDirectionalLights - NumPointLights],
			intersection,
			normal
		);
	}
}

#endif // LIGHTING_GLSL
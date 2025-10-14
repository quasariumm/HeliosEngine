#ifndef LIGHTING_CL_
#define LIGHTING_CL_

#include "core/common.cl"
#include "core/random.cl"

/*
	Stores the light classes and light sampling logic
*/

/*
	Directional light
*/

struct DirectionalLight
{
	float3 color;
	float3 direction;
	float intensity;
};

float3 SampleDirectionalLight(struct DirectionalLight* light, float3 intersection, float3 normal)
{
	float overlap = dot(-light->direction, normal);
	if (overlap < 0.f)
		return (float3)(0.f);
	return light->color * light->intensity * overlap;
}

/*
	PointLight
*/

struct PointLight
{
	float3 position;
	float3 color;
	float intensity;
};

float3 SamplePointLight(struct PointLight* light, float3 intersection, float3 normal)
{
	float diff = length(light->position - intersection);
	float overlap = dot(normal, (light->position - intersection) / diff);
	if (overlap < 0.f)
		return (float3)(0.f);
	return light->intensity * light->color * native_recip(diff * diff) * overlap;
}

/*
	Simple SpotLight
*/

struct SpotLight
{
	float3 position;
	float3 color;
	float3 direction;
	float intensity;
	float innerCutOff;
	float outerCutOff;
};

float3 SampleSpotLight(struct SpotLight* light, float3 intersection, float3 normal)
{
	float3 lightDir = normalize(light->position - intersection);
	float theta = dot(lightDir, -light->direction);

	if (theta < 0.f)
		return (float3)(0.f);

	float epsilon = (light->innerCutOff - light->outerCutOff);
	float intensity = clamp((theta - light->outerCutOff) / epsilon, 0.f, 1.f);

	return light->intensity * intensity * light->color * dot(normal, lightDir);
}


// Returns lightVector in first column, light in second
void SampleSceneLights(
    uint* seed, float3 intersection, float3 normal, 
    struct LightsContext* ctx,
    float3* outLightVec, float3* outLight
)
{
	uint numLights = ctx->numDirectionalLights + ctx->numPointLights + ctx->numSpotLights;

	if (numLights == 0)
		return;

	uint sampledLight = (uint)(trunc(Xi(seed) * numLights));
	if (sampledLight < ctx->numDirectionalLights)
	{
		struct DirectionalLight* light = &ctx->directionalLights[sampledLight];
		*outLightVec = normalize(-light->direction);
        *outLight = (float)(numLights) * SampleDirectionalLight(light, intersection, normal);
	}
	else if (sampledLight < ctx->numDirectionalLights + ctx->numPointLights)
	{
		struct PointLight* light = &ctx->pointLights[sampledLight - ctx->numDirectionalLights];
		*outLightVec = normalize(light->position - intersection);
        *outLight = (float)(numLights) * SamplePointLight(light, intersection, normal);
	}
	else if (sampledLight < ctx->numDirectionalLights + ctx->numPointLights + ctx->numSpotLights)
	{
		// TODO: Fix this not compiling
		struct SpotLight* light = &ctx->spotLights[sampledLight - ctx->numDirectionalLights - ctx->numPointLights];
		*outLightVec = normalize(light->position - intersection);
        *outLight = (float)(numLights) * SampleSpotLight(light, intersection, normal);
	}
}

#endif

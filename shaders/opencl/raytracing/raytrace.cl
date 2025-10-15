#pragma OPENCL EXTENSION cl_khr_fp16 : enable

#include "core/common.cl"
#include "raytracing/intersections.cl"
#include "raytracing/lighting.cl"

//#define REPROJECTION
//#define REPROJECTION_BILERP
#define ACCUMULATION

struct SkyboxInfo
{
	float3 groundColor;
	float3 horizonColor;
	float3 zenithColor;
	float3 sunDirection;

	float sunFocus;
	float sunIntensity;

	bool useSkyboxTexture;
	uint8 _padding[3];
};

float3 GetSkyboxColor(
    struct Ray* ray, 
    image2d_t skyboxTexture,
    struct SkyboxInfo* skyboxInfo
)
{
	if (skyboxInfo->useSkyboxTexture)
	{
		const float u = 0.5f * M_1_PI_F * atan2(ray->dir.z, ray->dir.x);
		const float v = M_1_PI_F * acos(ray->dir.y);

		if (u > 1.f || u < 0.f || v > 1.f || v < 0.f)
			return (float3)(0.f);

		const float3 pixel = sqrt(read_imagef(skyboxTexture, texSampler, (float2)(u, v)).xyz);

		return 0.55f * pixel;
	}
	else
	{
		float skyGradientT = pow(smoothstep(0.0f, 0.4f, ray->dir.y), 0.35f);
		float3 skyGradient = mix(skyboxInfo->horizonColor, skyboxInfo->zenithColor, skyGradientT);
		float sun = pow(max(0.f, dot(ray->dir, -skyboxInfo->sunDirection)), skyboxInfo->sunFocus) * skyboxInfo->sunIntensity;

		float groundToSkyT = smoothstep(-0.01f, 0.0f, ray->dir.y);
		float sunMask = 0.f;
		if (groundToSkyT >= 1.f)
			sunMask = 1.f;
		return mix(skyboxInfo->groundColor, skyGradient, groundToSkyT) + sun * sunMask;
	}
}

float3 Trace(
    struct Ray* ray, uint seed, struct Ray* firstRay,
    image2d_t skyboxTexture, struct SkyboxInfo* skyboxInfo,
    struct GeometryContext* geometryContext,
    struct LightsContext* lightsContext
)
{
	float3 incommingLight = (float3)(0.f);
	float3 rayColor = (float3)(1.f);
	float3 mixedColor = (float3)(1.f); /* For determining the light */

	for (int i = 0; i < 10; ++i)
	{
		RayCollision(ray, geometryContext);

		if (ray->hit.hit)
		{
			struct Material* material = &geometryContext->materials[ray->hit.materialIndex];

			float3 emittance = material->emissionColor * material->emissionStrength;

            float3 light;
			SampleSceneLights(
                &seed, ray->hit.hitPoint, ray->hit.normal,
                lightsContext,
                &ray->hit.lightVector, &light
            );

			// Shadow ray
			struct Ray shadowRay = {
				ray->hit.hitPoint + 0.001f * ray->hit.normal,
				ray->hit.lightVector,
				native_recip(ray->hit.lightVector)
            };
			RayCollision(&shadowRay, geometryContext);
			light *= (float)(!shadowRay.hit.hit);

			float3 BRDF = GetBRDFAndBounce(ray, material, &seed);
			float PDF = GetPDF(material, ray->hit.normal, ray->hit.tangent, -ray->dir, ray->hit.lightVector);

			if (PDF == 0.f)
				incommingLight += emittance * mixedColor;
			else
				incommingLight += emittance * mixedColor + BRDF / PDF * light * rayColor * max(0.001f, -dot(ray->hit.normal, ray->dir));

			float specularity = material->specularity;
			if ((material->properties & MATERIAL_MICROFACET) != 0)
			{
				specularity = 1.f - material->PBR_Roughness;
			}

			float3 color = mix(material->diffuseColor, material->specularColor, specularity);

			if (material->refractivity > 0.f)
			{
				if (ray->hit.inside)
				{
					// https://www.flipcode.com/archives/Raytracing_Topics_Techniques-Part_3_Refractions_and_Beers_Law.shtml (Author: Jacco Bikker)
					float3 absorbance = material->specularColor * material->absorption * -ray->hit.dst;
					float3 transmittance = exp(absorbance);
					color = transmittance;
				}
				else
				{
					// TODO: Change this to the correct calculation
					color = (float3)(1.f);
				}
			}

			mixedColor *= color;
			rayColor *= color * mix(light, (float3)(1.f), specularity);

			ray->origin = ray->hit.hitPoint + 0.001f * ray->hit.normal * sign(dot(ray->hit.normal, ray->dir));
		}
		else
		{
			incommingLight += mixedColor * GetSkyboxColor(ray, skyboxTexture, skyboxInfo);
			break;
		}
		if(i == 1) firstRay = ray;
	}

	return incommingLight;
}

float4 SamplePrevColor(float2 PixelCoord, float depth, float16 VPMat, float16 PrevVPMat, uint2 screenDimensions, image2d_t outImage, sampler_t texSampler)
{
	// Convert pixel coordinate and depth to NDC
	float4 currNDC = (float4)(PixelCoord * 2.f - 1.f, -depth/1e30f * 2.f - 1.f, 1.f);

	// Transform to world space
    float16 inv;
    inverse_mat4(VPMat, &inv);
	float4 worldPos = mat4_mul_vec4(inv, currNDC);
	worldPos /= worldPos.w;

	// Project to previous frame's screen space
	float4 prevNDC = mat4_mul_vec4(PrevVPMat, worldPos);
	float2 prevUV = (prevNDC.xy / prevNDC.w) * 0.5f + 0.5f;

	// Check bounds before sampling
	if (any(isless(prevUV, (float2)(0.f))) || any(isgreater(prevUV, (float2)(1.f))))
		return (float4)(0.f); // or handle out-of-bounds case

	// Convert to texel coordinates
	float2 prevTexelCoord = (float2)(prevUV.x * screenDimensions.x, prevUV.y * screenDimensions.y);
	float2 tmp = round(prevTexelCoord);
    int2 prevPixelCoord = (int2)((int)tmp.x, (int)tmp.y);

	return read_imagef(outImage, texSampler, prevPixelCoord);
}

__kernel void Raytrace(
    __read_write image2d_t outImage, uint2 screenDimensions, float16 camToWorld, float16 vp, float16 prevVP, float3 viewParams, ulong frame,
    __read_only image2d_t skyboxTexture, __global struct SkyboxInfo* skyboxInfo,
    __global struct GeometryContext* geometryContext,
    __global struct LightsContext* lightsContext
)
{
    size_t id = get_global_id(0);
	int2 texelCoord = (int2)(id % screenDimensions.x, id / screenDimensions.x);
	
	if (texelCoord.x >= screenDimensions.x || texelCoord.y >= screenDimensions.y)
		return;

	float2 tc = (float2)(texelCoord.x, texelCoord.y) / (float2)(screenDimensions.x, screenDimensions.y);

	write_imagef(outImage, texelCoord, (float4)(tc, 0.f, 1.f));
	return;

	float3 focusDirLocal = (float3)((tc - 0.5f) * viewParams.xy, viewParams.z);
	float3 focusDir = normalize(transform_point(camToWorld, focusDirLocal).xyz);
	struct Ray ray = {
		(float3)(camToWorld.s3, camToWorld.s7, camToWorld.sb),
		focusDir,
		native_recip(focusDir),
	};

	uint seed = (uint)(floor(tc.y * screenDimensions.y * screenDimensions.x + tc.x * screenDimensions.x));
	seed ^= WangHash((uint*)&frame);
	seed = WangHash(&seed);
	
	struct Ray firstRay; 
	
	float4 pixel = (float4)(Trace(&ray, seed, &firstRay, skyboxTexture, skyboxInfo, geometryContext, lightsContext), 1.f);
	pixel = max(pixel, (float4)(0.f, 0.f, 0.f, 1.f));
    
    // #ifdef REPROJECTION
	// if (!ClearAccumulator && firstRay.hit.didHit)
    // {
	// 	vec4 accumulatorColor;
		
	// 	#ifdef REPROJECTION_BILERP
	// 	//Bilinear interpolation
	// 	if (tc.x >= 0 && tc.x < ScreenWidth - 2 && tc.y >= 0 && tc.y < ScreenHeight - 2)
	// 	{
	// 		vec4 currNDC = vec4(tc * 2.0 - 1.0, firstRay.hit.dst, 1.0);
	// 		vec4 worldPos = inverse(VPMat) * currNDC;
	// 		worldPos /= worldPos.w;

	// 		vec4 prevNDC = PrevVPMat * worldPos;
	// 		vec2 prevUV = (prevNDC.xy / prevNDC.w) * 0.5 + 0.5;
	// 		vec2 clampedPrevUV = clamp(prevUV, vec2(0.0), vec2(1.0 - 1.0 / vec2(ScreenWidth, ScreenHeight)));
	// 		ivec2 prevTexelCoord = ivec2(clampedPrevUV * vec2(ScreenWidth, ScreenHeight));
			
	// 		float fx = fract( prevTexelCoord.x ), fy = fract( prevTexelCoord.y );
	// 		float w0 = (1 - fx) * (1 - fy);
	// 		float w1 = fx * (1 - fy);
	// 		float w2 = (1 - fx) * fy;
	// 		float w3 = fx * fy;
			
	// 		vec4 p0 = SamplePrevColor(tc, firstRay.hit.dst); // (x,y)
	// 		vec4 p1 = SamplePrevColor(vec2(tc.x + 1, tc.y), firstRay.hit.dst); // (x+1,y)
	// 		vec4 p2 = SamplePrevColor(vec2(tc.x, tc.y + 1), firstRay.hit.dst); // (x,y+1)
	// 		vec4 p3 = SamplePrevColor(vec2(tc.x + 1, tc.y+ 1), firstRay.hit.dst); // (x+1,y+1)
	// 		accumulatorColor = p0 * w0 + p1 * w1 + p2 * w2 + p3 * w3;
	// 	}
	// 	else
	// 	{
	// 		accumulatorColor = SamplePrevColor(tc, firstRay.hit.dst); // (x,y)
	// 	}
	// 	#else //REPROJECTION_NO_BILERP

	// 	accumulatorColor = SamplePrevColor(tc, firstRay.hit.dst); // (x,y)
		
	// 	#endif //REPROJECTION_BILERP
		
    //     pixel = mix(accumulatorColor, pixel, 0.01);
    //     pixel.a = 1.0;
    // }
    // #elif defined(ACCUMULATION)
	if (frame != 0)
    {
        float4 accumulatorColor = read_imagef(outImage, texelCoord);
        pixel = mix(accumulatorColor, pixel, 1.f / (float)(frame + 1));
        pixel.a = 1.0;
    }
    // #endif // ACCUMULATION

	write_imagef(outImage, texelCoord, pixel);
}


/*
 *  Gets the index of the nearest intersected object. TypeFound maps to: 0 -> Sphere, 1 -> Mesh
 */
__kernel void GetIntersectionIdx(
        float16 camToWorld, float3 viewParams, uint2 localPixel, uint2 screenDimensions, 
        __global struct GeometryContext* ctx, 
        __global uint* sphereIdx, __global uint* meshIdx, __global uint* typeFound
)
{
    float2 tc = (float2)(localPixel.x, localPixel.y) / (float2)(screenDimensions.x, screenDimensions.y);

    float3 focusDirLocal = (float3)((tc - 0.5f) * viewParams.xy, viewParams.z);
	float3 focusDir = normalize(transform_point(camToWorld, focusDirLocal).xyz);
	struct Ray ray = {
		(float3)(camToWorld.s3, camToWorld.s7, camToWorld.sb),
		focusDir,
		native_recip(focusDir),
	};

    ray.hit.hit = false;
	ray.hit.dst = 1e30;

    float closestDst = ray.hit.dst;


	for (int i = 0; i < ctx->numSpheres; ++i)
    {
        bool res = RaySphere(&ray, &ctx->spheres[i]);
        if (res && ray.hit.dst < closestDst)
        {
            *typeFound = 0;
            *sphereIdx = i;
            closestDst = ray.hit.dst;
        }
    }

	for (uint i = 0; i < ctx->numMeshes; ++i)
    {
        bool res = RayMesh(&ray, &ctx->meshes[i], ctx->bvhNodes, ctx->vertices, ctx->indices);
        if (res && ray.hit.dst < closestDst)
        {
            *typeFound = 1;
            *meshIdx = i;
            closestDst = ray.hit.dst;
        }
    }
}

#ifndef MATERIALS_GLSL
#define MATERIALS_GLSL

#include "/Engine/Core/random.glsl"
#include "/Engine/Raytracing/ray.glsl"

/*
	Houses the material struct and material-related functions
*/

#define MATERIAL_REFLECTION 	1
#define MATERIAL_TRANSMISSION 	2
#define MATERIAL_DIFFUSE 		4
#define MATERIAL_GLOSSY 		8
#define MATERIAL_SPECULAR 		16
#define MATERIAL_ALL 			MATERIAL_REFLECTION | MATERIAL_TRANSMISSION | MATERIAL_DIFFUSE | MATERIAL_GLOSSY | MATERIAL_SPECULAR

struct RayTracingMaterial
{
	int type;
	vec3 diffuseColor;
	vec3 specularColor;
	float specularity;
	float shininess;
	float glossiness;

	vec3 emissionColor;
	float emissionStrength;

	float refractivity;
	float refractionCoefficient;
};

const RayTracingMaterial defaultMaterial = RayTracingMaterial(MATERIAL_ALL, vec3(0.0), vec3(0.0), 0.0, 0.0, 0.0, vec3(0.0), 0.0, 0.0, 1.0);

/*
	This goes in here for dependecy reasons
*/
struct RayHitInfo
{
	bool didHit;
	float dst;
	vec3 hitPoint;
	vec3 normal;
	vec3 lightVector;
	RayTracingMaterial material;
};

const RayHitInfo defaultHitInfo = RayHitInfo( false, 1e30, vec3(0.0), vec3(0.0), vec3(0.0), defaultMaterial );

/*
	Bounces
*/

vec3 LambertianBounce(inout uint seed, in vec3 normal)
{
	return normalize(normal + RandomHemisphericalDirection(seed));
}

vec3 Reflect(in vec3 wo, in vec3 normal)
{
	return normalize(-wo + 2.0 * dot(wo, normal) * normal);
}

// Returns direction in xyz, Fresnel term in w
// TODO: Support for refraction from non-air to non-air
vec4 Refract(in vec3 wo, in vec3 normal, in float etaT, out float outEta)
{
	float eta = etaT;
	vec3 N = normal;
	float cosThetaI = dot(N, wo);

	if (cosThetaI < 0.0)
	{
		eta = 1.0 / eta;
		cosThetaI = -cosThetaI;
		N = -N;
	}

	float sin2ThetaI = max(0.0, 1.0 - (cosThetaI * cosThetaI));

	float sin2ThetaT = sin2ThetaI / (eta * eta);
	if (sin2ThetaT >= 1.0)
		return vec4(1.0); // TIR

	float cosThetaT = sqrt(1.0 - sin2ThetaT);

	float R0 = (1.0 - etaT) / (1.0 + etaT);
	R0 *= R0;
	float x = (1.0 - cosThetaI);
	float fresnel = R0 + (1.0 - R0) * x * x * x * x * x;

	outEta = eta;
	return vec4(normalize(-wo / eta + (cosThetaI / eta - cosThetaT) * N), fresnel);
}

/*
	BRDF, BSSDF and PDF
*/

struct TypeInfo
{
	float factor;
	vec3 direction;
	vec3 BRDF;
};

// These functions are combined for the reason that the same values need to be calculated eitherway
vec3 GetBRDFAndBounce(inout RayTracingMaterial material, inout Ray ray, inout uint seed, in RayHitInfo info)
{
	TypeInfo diffuse 		= TypeInfo(0.0, vec3(0.0), vec3(0.0));
	TypeInfo specular 		= TypeInfo(0.0, vec3(0.0), vec3(0.0));
	TypeInfo transmission 	= TypeInfo(0.0, vec3(0.0), vec3(0.0));

	bool glassReflect = false;

	if ((material.type | MATERIAL_DIFFUSE) != 0
		&& material.specularity < 1.0)
	{
		diffuse.factor = 1.0 - material.specularity;
		if ((material.type | MATERIAL_REFLECTION) != 0)
			diffuse.direction = LambertianBounce(seed, info.normal);

		diffuse.BRDF = material.diffuseColor * INVPI;
	}
	if ((material.type | MATERIAL_SPECULAR) != 0
		&& material.specularity > 0.0)
	{
		specular.factor = material.specularity;
		if ((material.type | MATERIAL_REFLECTION) != 0)
			specular.direction = Reflect(-ray.dir, info.normal);
		// https://www.cs.cmu.edu/afs/cs/academic/class/15462-f09/www/lec/lec8.pdf
		vec3 H = normalize(info.lightVector - ray.dir);
		float overlap = max(0.0, dot(H, info.normal));
		specular.BRDF = material.specularColor * pow(overlap, material.shininess);
	}
	if ((material.type | MATERIAL_TRANSMISSION) != 0
		&& material.refractivity > 0.0)
	{
		transmission.factor = material.refractivity;
		if (material.refractionCoefficient == 1.0
			|| ((material.type | MATERIAL_REFLECTION) != 0))
		{
			float outEta = 0.0;
			vec4 res = Refract(-ray.dir, info.normal, material.refractionCoefficient, outEta);
			float fresnel = res.w;
			if (Xi(seed) <= fresnel)
				glassReflect = true;

			transmission.direction = res.xyz;
		}
	}

	// Temp
	if (material.refractivity > 0.0)
	{
		if (glassReflect)
			ray.dir = mix(diffuse.direction, specular.direction, material.specularity);
		else
			ray.dir = mix(diffuse.direction, transmission.direction, material.specularity);
	}
	else
	{
		ray.dir = mix(diffuse.direction, specular.direction, specular.factor);
	}
	return mix(diffuse.BRDF, specular.BRDF, specular.factor);
}


float GetPDF(inout RayTracingMaterial material)
{
	if (material.specularity == 1.0)
	{
		return 1.0;
	}
	return INVPI;
}

#endif // MATERIALS_GLSL
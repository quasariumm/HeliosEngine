#ifndef MATERIALS_GLSL
#define MATERIALS_GLSL

#include "/Engine/Core/random.glsl"
#include "/Engine/Raytracing/ray.glsl"

/*
	Houses the material struct and material-related functions
*/

#define MATERIAL_REFLECTION 	1
#define MATERIAL_MICROFACET		2
#define MATERIAL_TRANSMISSION 	4
#define MATERIAL_DIFFUSE 		8
#define MATERIAL_GLOSSY 		16
#define MATERIAL_SPECULAR 		32
#define MATERIAL_ALL 			MATERIAL_REFLECTION | MATERIAL_MICROFACET | MATERIAL_TRANSMISSION | MATERIAL_DIFFUSE | MATERIAL_GLOSSY | MATERIAL_SPECULAR

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
vec4 Refract(in vec3 wo, in vec3 normal, in float etaT, out float etaO, out float etaI)
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

	if (eta == etaT)
	{
		etaI = 1.0;
		etaO = etaT;
	}
	else
	{
		etaI = etaT;
		etaO = 1.0;
	}
	return vec4(normalize(-wo / eta + (cosThetaI / eta - cosThetaT) * N), fresnel);
}

/*
	BRDF, BSSDF and PDF
*/

struct BRDFInfo
{
	float factor;
	vec3 direction;
	vec3 BRDF;
};

// These functions are combined for the reason that the same values need to be calculated eitherway
vec3 GetBRDFAndBounce(inout RayTracingMaterial material, inout Ray ray, inout uint seed, in RayHitInfo info)
{
	BRDFInfo diffuse 		= BRDFInfo(0.0, vec3(0.0), vec3(0.0));
	BRDFInfo specular 		= BRDFInfo(0.0, vec3(0.0), vec3(0.0));
	BRDFInfo transmission 	= BRDFInfo(0.0, vec3(0.0), vec3(0.0));

	bool glassReflect = false;

	if ((material.type | MATERIAL_DIFFUSE) != 0
		&& material.specularity < 1.0)
	{
		diffuse.factor = 1.0 - material.specularity;
		// Direction
		if ((material.type | MATERIAL_REFLECTION) != 0)
			diffuse.direction = LambertianBounce(seed, info.normal);

		// BRDF
		diffuse.BRDF = material.diffuseColor * INVPI;
	}
	if ((material.type | MATERIAL_SPECULAR) != 0
		&& material.specularity > 0.0)
	{
		specular.factor = material.specularity;
		// Direction
		if ((material.type | MATERIAL_REFLECTION) != 0)
			specular.direction = Reflect(-ray.dir, info.normal);
		// BRDF
		// https://www.cs.cmu.edu/afs/cs/academic/class/15462-f09/www/lec/lec8.pdf
		vec3 H = normalize(info.lightVector - ray.dir);
		float overlap = max(0.0, dot(H, info.normal));
		specular.BRDF = material.specularColor * pow(overlap, material.shininess);
	}
	if ((material.type | MATERIAL_TRANSMISSION) != 0
		&& material.refractivity > 0.0)
	{
		transmission.factor = material.refractivity;

		if ((material.type | MATERIAL_REFLECTION) != 0)
		{
			float etaI = 1.0;
			float etaO = material.refractionCoefficient;
			// Direction
			vec4 res = Refract(-ray.dir, info.normal, material.refractionCoefficient, etaI, etaO);
			float fresnel = res.w;
			if (Xi(seed) <= mix(1.0, fresnel, transmission.factor))
				glassReflect = true;
			specular.factor *= float(glassReflect);

			transmission.direction = res.xyz;
			// BRDF
			// https://cgg.mff.cuni.cz/~jaroslav/teaching/2017-npgr010/slides/03%20-%20npgr010-2017%20-%20BRDF.pdf
			float overlap = max(0.0, -dot(res.xyz, ray.dir));
			float changeOfRadiance = (etaO * etaO) / (etaI * etaI);
			// Physically-plausible Phong Distributuion
			vec3 DPPPhong = info.material.specularColor * ((material.shininess + 2.0) / TWO_PI) * pow(overlap, material.shininess);
			transmission.BRDF = changeOfRadiance * (1.0 - fresnel) * DPPPhong / dot(info.normal, info.lightVector);
			transmission.factor *= float(!glassReflect);
		}
	}

	ray.dir = diffuse.direction * diffuse.factor + specular.direction * specular.factor + transmission.direction * transmission.factor;
	return diffuse.BRDF * diffuse.factor + specular.BRDF * specular.factor + transmission.BRDF * transmission.factor;
}

struct PDFInfo
{
	float factor;
	float PDF;
};

float GetPDF(inout RayTracingMaterial material)
{
	PDFInfo diffuse = PDFInfo(
		1.0 - material.specularity,
		INVPI
	);
	PDFInfo specularTransmission = PDFInfo(
		material.specularity,
		1.0
	);
	return diffuse.PDF * diffuse.factor + specularTransmission.PDF * specularTransmission.factor;
}

#endif // MATERIALS_GLSL
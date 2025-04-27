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
	float shininess;
	float specularProbability;

	vec3 emissionColor;
	float emissionStrength;

	float refractionProbability;
	float refractionCoefficient;
};

const RayTracingMaterial defaultMaterial = RayTracingMaterial(MATERIAL_ALL, vec3(0.0), vec3(0.0), 0.0, 1.0, vec3(0.0), 0.0, 0.0, 1.0);

/*
	This goes in here for dependecy reasons
*/
struct RayHitInfo
{
	bool didHit;
	float dst;
	vec3 hitPoint;
	vec3 normal;
	RayTracingMaterial material;
};

const RayHitInfo defaultHitInfo = RayHitInfo( false, 1e30, vec3(0.0), vec3(0.0), defaultMaterial );

/*
	BRDF, BSSDF and PDF
*/

vec3 GetBRDF(inout RayTracingMaterial material)
{
	return material.diffuseColor * INVPI;
}


float GetPDF(inout RayTracingMaterial material)
{
	return INVPI;
}

/*
	Bounces
*/

void LambertianBounce(inout Ray ray, inout uint seed, in RayHitInfo info)
{
	ray.dir = normalize(info.normal + RandomHemisphericalDirection(seed));
	ray.invDir = 1.0 / ray.dir;
}

#endif // MATERIALS_GLSL
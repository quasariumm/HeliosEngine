#ifndef MATERIAL_STRUCT_GLSL
#define MATERIAL_STRUCT_GLSL

/*
	Houses the material struct
*/

struct RayTracingMaterial
{
	vec3 diffuseColor;
	float specularity;
	vec3 specularColor;
	float shininess;
	vec3 emissionColor;
	float emissionStrength;

	int type;
	float glossiness;

	float refractivity;
	float refractionCoefficient;
	float absorption;

	float PBR_Roughness;
	float PBR_Metallic;

	float alphaX;
	float alphaY;
	float _padding1;
	float _padding2;
	float _padding3;
};

/*
	Houses material-related functions
*/

#define MATERIAL_REFLECTION 	1
#define MATERIAL_MICROFACET		2
#define MATERIAL_TRANSMISSION 	4
#define MATERIAL_DIFFUSE 		8
#define MATERIAL_GLOSSY 		16
#define MATERIAL_SPECULAR 		32

#define MATERIAL_DEFAULT 		MATERIAL_REFLECTION | MATERIAL_TRANSMISSION | MATERIAL_DIFFUSE | MATERIAL_GLOSSY | MATERIAL_SPECULAR// | MATERIAL_MICROFACET | MICROFACET_BECKMANN

layout (std430, binding = 8) readonly buffer MaterialsBuffer
{
	RayTracingMaterial Materials[];
};

#endif // MATERIAL_STRUCT_GLSL
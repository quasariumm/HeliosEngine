#ifndef MATERIAL_STRUCT_GLSL
#define MATERIAL_STRUCT_GLSL

/*
	Houses the material struct
*/

struct RayTracingMaterial
{
	uint materialProperties;

	float alphaX;
	float alphaY;

	float specularity;
	vec3 diffuseColor;
	float shininess;
	vec3 specularColor;
	vec3 emissionColor;
	float emissionStrength;

	float refractivity;
	float refractionCoefficient;
	float absorption;

	float PBR_Roughness;
	float PBR_Metallic;

	float padding[3];
};

/*
	Houses material-related functions
*/

#define MATERIAL_REFLECTION 	0x01
#define MATERIAL_MICROFACET		0x02
#define MATERIAL_TRANSMISSION 	0x04
#define MATERIAL_DIFFUSE 		0x08
#define MATERIAL_GLOSSY 		0x10
#define MATERIAL_SPECULAR 		0x20

layout (std430, binding = 8) readonly buffer MaterialsBuffer
{
	RayTracingMaterial Materials[];
};

#endif // MATERIAL_STRUCT_GLSL
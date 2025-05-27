#ifndef MATERIAL_STRUCT_GLSL
#define MATERIAL_STRUCT_GLSL

/*
	Houses the material struct
*/

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

	float PBR_Roughness;
	float PBR_Metallic;
	float PBR_Reflectance;

	float alphaX;
	float alphaY;
};

#endif // MATERIAL_STRUCT_GLSL
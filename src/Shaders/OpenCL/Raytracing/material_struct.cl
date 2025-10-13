#ifndef MATERIAL_STRUCT_CL_
#define MATERIAL_STRUCT_CL_

struct Material
{
    float3 diffuseColor;
	float specularity;
	float3 specularColor;
	float shininess;
	float3 emissionColor;
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

#define MATERIAL_REFLECTION 	1
#define MATERIAL_MICROFACET		2
#define MATERIAL_TRANSMISSION 	4
#define MATERIAL_DIFFUSE 		8
#define MATERIAL_GLOSSY 		16
#define MATERIAL_SPECULAR 		32

#define MATERIAL_DEFAULT 		MATERIAL_REFLECTION | MATERIAL_TRANSMISSION | MATERIAL_DIFFUSE | MATERIAL_GLOSSY | MATERIAL_SPECULAR

#endif
#ifndef MATERIAL_STRUCT_CL_
#define MATERIAL_STRUCT_CL_

struct Material
{
    uint properties;
    uint microfacetModel;   // <-
	float alphaX;           // <-   These are packed in a 12-byte struct, but here I unpack them
	float alphaY;           // <-
    float3 diffuseColor;
	float specularity;
	float3 specularColor;
	float shininess;
	float3 emissionColor;
	float emissionStrength;

	float glossiness;

	float refractivity;
	float refractionCoefficient;
	float absorption;

	float PBR_Roughness;
	float PBR_Metallic;
};

#define MATERIAL_REFLECTION 	0x80
#define MATERIAL_MICROFACET		0x40
#define MATERIAL_TRANSMISSION 	0x20
#define MATERIAL_DIFFUSE 		0x10
#define MATERIAL_GLOSSY 		0x08
#define MATERIAL_SPECULAR 		0x04

#endif

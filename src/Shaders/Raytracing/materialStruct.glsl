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

const RayTracingMaterial defaultMaterial = RayTracingMaterial(
	MATERIAL_DEFAULT, 		// type
	vec3(0.0), vec3(0.0), 	// albedo
	0.0, 0.0, 0.0, 			// specular parameters
	vec3(0.0), 0.0, 		// emission
	0.0, 1.0, 				// transmission/dielectric
	0.3, 0.3, 0.3,			// PBR
	0.0, 0.0				// Anisotropic alphas
);

#endif // MATERIAL_STRUCT_GLSL
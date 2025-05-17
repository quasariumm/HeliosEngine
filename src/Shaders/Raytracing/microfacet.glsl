#ifndef MICROFACET_GLSL
#define MICROFACET_GLSL

/*
	Houses the material struct and material-related functions
*/

#include "/Engine/Raytracing/materialStruct.glsl"

// EXTENDS UPON MATERIAL_XXXX. THIS IS INCLUDED IN THE MATERIAL TYPE
#define MICROFACET_BECKMANN		64		// Beckmann–Spizzichino model
#define MICROFACET_GGX_ISO		128		// Trowbridge–Reitz 'GGX' model
#define MICROFACET_GGX_ANISO	256		// Anisotropic version of GGX model
#define MICROFACET_BLINNPHONG	512		// Blinn-Phong model

float BeckmannD(float alpha, vec3 normal, vec3 wh);
float BeckmannG1(float alpha, vec3 normal, vec3 w);
float BeckmannG(float alpha, vec3 normal, vec3 wo, vec3 wi);

float GGXIsoD(float alpha, vec3 normal, vec3 wh);
float GGXIsoG1(float alpha, vec3 normal, vec3 w);
float GGXIsoG(float alpha, vec3 normal, vec3 wo, vec3 wi);

float GGXAnisoD(float alpha, vec3 normal, vec3 wh);
float GGXAnisoG1(float alpha, vec3 normal, vec3 w);
float GGXAnisoG(float alpha, vec3 normal, vec3 wo, vec3 wi);

float BlinnPhongD(float alpha, vec3 normal, vec3 wh);
float BlinnPhongG1(float alpha, vec3 normal, vec3 w);
float BlinnPhongG(float alpha, vec3 normal, vec3 wo, vec3 wi);

vec3 FresnelSchlick(float cosTheta, vec3 F0);

vec3 MicrofacetBRDF(RayTracingMaterial material, vec3 normal, vec3 wo, vec3 wi)
{
	const vec3 wh = normalize(wo + wi);
	const vec3 F0 = mix( vec3(0.16 * material.PBR_Reflectance * material.PBR_Reflectance), material.diffuseColor, material.PBR_Metallic );

	float alpha = material.PBR_Roughness * material.PBR_Roughness;

	float DG = 0.0;
	if ((material.type & MICROFACET_BECKMANN) != 0)
	{
		DG = BeckmannD(alpha, normal, wh) * BeckmannG(alpha, normal, wo, wi);
	}
	if ((material.type & MICROFACET_GGX_ISO) != 0)
	{
		DG = GGXIsoD(alpha, normal, wh) * GGXIsoG(alpha, normal, wo, wi);
	}
	if ((material.type & MICROFACET_GGX_ANISO) != 0)
	{
		DG = GGXAnisoD(alpha, normal, wh) * GGXAnisoG(alpha, normal, wo, wi);
	}
	if ((material.type & MICROFACET_BLINNPHONG) != 0)
	{
		DG = BlinnPhongD(alpha, normal, wh) * BlinnPhongG(alpha, normal, wo, wi);
	}

	vec3 F = FresnelSchlick(dot(normal, wo), F0);
	vec3 spec = (DG * F) / (4.0 * abs(dot(normal, wo)) * abs(dot(normal, wi)));

	vec3 diff = (1.0 - F) * (1.0 - material.PBR_Metallic) * material.specularColor * INVPI;

	return spec + diff;
}

float MicrofacetPDF(RayTracingMaterial material, vec3 normal, vec3 wo, vec3 wi)
{
	const vec3 wh = normalize(wo + wi);
	float alpha = material.PBR_Roughness * material.PBR_Roughness;

	float DG1 = 0.0;
	if ((material.type & MICROFACET_BECKMANN) != 0)
	{
		DG1 = BeckmannD(alpha, normal, wh) * BeckmannG1(alpha, normal, wo);
	}
	if ((material.type & MICROFACET_GGX_ISO) != 0)
	{
		DG1 = GGXIsoD(alpha, normal, wh) * GGXIsoG1(alpha, normal, wo);
	}
	if ((material.type & MICROFACET_GGX_ANISO) != 0)
	{
		DG1 = GGXAnisoD(alpha, normal, wh) * GGXAnisoG1(alpha, normal, wo);
	}
	if ((material.type & MICROFACET_BLINNPHONG) != 0)
	{
		DG1 = BlinnPhongD(alpha, normal, wh) * BlinnPhongG1(alpha, normal, wo);
	}

	return DG1 * abs(dot(wo, wh)) / abs(dot(normal, wo));
}


vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * cosTheta * cosTheta * cosTheta * cosTheta * cosTheta;
}

/****************************************************************************
 *	  				Microfacet model implementations						*
 *	https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html	*
 ****************************************************************************/

/*
	Beckmann
*/

float BeckmannD(float alpha, vec3 normal, vec3 wh)
{
	float NdotH = dot(normal, wh);
	float NdotH2 = NdotH * NdotH;
	float NdotH4 = NdotH2 * NdotH2;
	float alpha2 = alpha * alpha;
	return 1.0 / (alpha2 * NdotH4) * exp((NdotH2 - 1.0) / (alpha2 * NdotH2));
}

float BeckmannG1(float alpha, vec3 normal, vec3 w)
{
	float k = alpha * sqrt(2.0 / PI);
	float NdotV = dot(normal, w);
	return NdotV / (NdotV * (1.0 - k) + k);
}

float BeckmannG(float alpha, vec3 normal, vec3 wo, vec3 wi)
{
	return BeckmannG1(alpha, normal, wo) * BeckmannG1(alpha, normal, wi);
}

/*
	Isotropic GGX
*/

float GGXIsoD(float alpha, vec3 normal, vec3 wh)
{
	float alpha2 = alpha * alpha;
	float NdotH = dot(normal, wh);

	float f = NdotH * NdotH * (alpha2 - 1.0) + 1.0;

	return alpha2 / (PI * f * f);
}

float GGXIsoG1(float alpha, vec3 normal, vec3 w)
{
	float NdotV = dot(normal, w);
	float alpha2 = alpha * alpha;
	return (2.0 * NdotV) / (NdotV + sqrt( alpha2 + (1.0 - alpha2) * NdotV * NdotV ));
}

float GGXIsoG(float alpha, vec3 normal, vec3 wo, vec3 wi)
{
	return GGXIsoG1(alpha, normal, wo) * GGXIsoG1(alpha, normal, wi);
}

/*
	Anisotropic GGX
*/

float GGXAnisoD(float alpha, vec3 normal, vec3 wh)
{
	return 0.0; // TODO: Get alphaX and alphaY
}

float GGXAnisoG1(float alpha, vec3 normal, vec3 w)
{
	return GGXIsoG1(alpha, normal, w);
}

float GGXAnisoG(float alpha, vec3 normal, vec3 wo, vec3 wi)
{
	return GGXAnisoG1(alpha, normal, wo) * GGXAnisoG1(alpha, normal, wi);
}

/*
	Blinn-Phong
*/

float BlinnPhongD(float alpha, vec3 normal, vec3 wh)
{
	float alpha2 = alpha * alpha;

	return 1.0 / (PI * alpha2) * pow( dot(normal, wh), 2.0 / alpha2 - 2.0 );
}

float BlinnPhongG1(float alpha, vec3 normal, vec3 w)
{
	// The Smith integral has no closed form solution for Blinn-Phong. Walter [4] suggests using the same equation as Beckmann.
	return BeckmannG1(alpha, normal, w);
}

float BlinnPhongG(float alpha, vec3 normal, vec3 wo, vec3 wi)
{
	return BlinnPhongG1(alpha, normal, wo) * BlinnPhongG1(alpha, normal, wi);
}


#endif // MICROFACET_GLSL
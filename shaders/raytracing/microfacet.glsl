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

float GGXAnisoD(vec3 alpha, vec3 normal, vec3 tangent, vec3 wh);
float GGXAnisoG1(vec3 alpha, vec3 normal, vec3 tangent, vec3 w);
float GGXAnisoG(vec3 alpha, vec3 normal, vec3 tangent, vec3 wo, vec3 wi);

float BlinnPhongD(float alpha, vec3 normal, vec3 wh);
float BlinnPhongG1(float alpha, vec3 normal, vec3 w);
float BlinnPhongG(float alpha, vec3 normal, vec3 wo, vec3 wi);

vec3 FresnelSchlick(float cosTheta, vec3 F0);

vec3 MicrofacetBRDF(RayTracingMaterial material, vec3 normal, vec3 tangent, vec3 wo, vec3 wi)
{
	if (dot(normal, wo) == 0.0 || dot(normal, wi) == 0.0) return vec3(0.0);

	const vec3 wh = normalize(wo + wi);
	const vec3 F0 = mix( vec3(0.16 * material.PBR_Reflectance * material.PBR_Reflectance), material.specularColor, material.PBR_Metallic );

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
		vec3 anisoAlpha = vec3(material.alphaX, material.alphaY, sqrt(material.alphaX * material.alphaY));
		DG = GGXAnisoD(anisoAlpha, normal, tangent, wh) * GGXAnisoG(anisoAlpha, normal, tangent, wo, wi);
	}
	if ((material.type & MICROFACET_BLINNPHONG) != 0)
	{
		DG = BlinnPhongD(alpha, normal, wh) * BlinnPhongG(alpha, normal, wo, wi);
	}

	vec3 F = FresnelSchlick(dot(normal, wo), F0);
	return (DG * F) / (4.0 * dot(normal, wo) * dot(normal, wi));
}

float MicrofacetPDF(RayTracingMaterial material, vec3 normal, vec3 tangent, vec3 wo, vec3 wi)
{
	if (dot(wo, wi) < 0.0) return 0.0;

	const vec3 wh = normalize(wo + wi);
	float alpha = material.PBR_Roughness * material.PBR_Roughness;

	float D = 0.0;
	float G1 = 0.0;
	if ((material.type & MICROFACET_BECKMANN) != 0)
	{
		D = BeckmannD(alpha, normal, wh);
		G1 = BeckmannG1(alpha, normal, wo);
	}
	if ((material.type & MICROFACET_GGX_ISO) != 0)
	{
		D = GGXIsoD(alpha, normal, wh);
		G1 = GGXIsoG1(alpha, normal, wo);
	}
	if ((material.type & MICROFACET_GGX_ANISO) != 0)
	{
		vec3 anisoAlpha = vec3(material.alphaX, material.alphaY, sqrt(material.alphaX * material.alphaY));
		D = GGXAnisoD(anisoAlpha, normal, tangent, wh);
		G1 = GGXAnisoG1(anisoAlpha, normal, tangent, wo);
	}
	if ((material.type & MICROFACET_BLINNPHONG) != 0)
	{
		D = BlinnPhongD(alpha, normal, wh);
		G1 = BlinnPhongG1(alpha, normal, wo);
	}

	float PDF = G1 / abs(dot(normal, wo)) * D * abs(dot(wo, wh));
	return PDF / (4.0 * dot(wo, wh));
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
	float NdotH = max(0.001, dot(normal, wh));
	float NdotH2 = NdotH * NdotH;
	float NdotH4 = NdotH2 * NdotH2;
	float alpha2 = alpha * alpha;
	return 1.0 / (PI * alpha2 * NdotH4) * exp((NdotH2 - 1.0) / (alpha2 * NdotH2));
}

float BeckmannG1(float alpha, vec3 normal, vec3 w)
{
	float NdotV = max(0.001, dot(normal, w));
	float c = abs(NdotV) / (alpha * sqrt(1.0 - NdotV * NdotV));
	if (c < 1.6)
	{
		return (3.535 * c + 2.181 * c * c) / (1.0 + 2.276 * c + 2.577 * c * c);
	}
	else
	{
		return 1.0;
	}
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
	float NdotH = max(0.001, dot(normal, wh));

	float f = NdotH * NdotH * (alpha2 - 1.0) + 1.0;

	return alpha2 / (PI * f * f);
}

float GGXIsoG1(float alpha, vec3 normal, vec3 w)
{
	float NdotV = max(0.001, dot(normal, w));
	float alpha2 = alpha * alpha;
	return (2.0 * NdotV) / (NdotV + sqrt( alpha2 + (1.0 - alpha2) * NdotV * NdotV ));
}

float GGXIsoG(float alpha, vec3 normal, vec3 wo, vec3 wi)
{
	return GGXIsoG1(alpha, normal, wo) * GGXIsoG1(alpha, normal, wi);
}

/*
	Anisotropic GGX
	alpha is in the form { alphaX, alhpaY, alpha }
*/

// https://pbr-book.org/4ed/Geometry_and_Transformations/Spherical_Geometry#CosPhi
float CosPhi(vec3 w) {
	float sinTheta = sqrt(max(0.0, 1.0 - w.y * w.y));
	return (sinTheta == 0.0) ? 1.0 : clamp(w.x / sinTheta, -1.0, 1.0);
}
float SinPhi(vec3 w) {
	float sinTheta = sqrt(max(0.0, 1.0 - w.y * w.y));
	return (sinTheta == 0.0) ? 0.0 : clamp(w.z / sinTheta, -1.0, 1.0);
}

float GGXAnisoD(vec3 alpha, vec3 normal, vec3 tangent, vec3 wh)
{
	// Convert wh to tangent space
	vec3 wh_tangent = normalize(vec3(dot(wh, tangent) * alpha.x, dot(wh, normal), dot(wh, cross(tangent, normal)) * alpha.y));
	float cosTheta = wh_tangent.y;
	float cos2Theta = cosTheta * cosTheta;
	float cos4Theta = cosTheta * cosTheta * cosTheta * cosTheta;

	float sinTheta = max(0.0, 1.0 - cos2Theta);
	float tan2Theta = cos2Theta / (sinTheta * sinTheta);

	float cosPhi = CosPhi(wh_tangent);
	float sinPhi = SinPhi(wh_tangent);

	float c = PI * alpha.x * alpha.y;
	float x = (cosPhi * cosPhi) / (alpha.x * alpha.x);
	float y = (sinPhi * sinPhi) / (alpha.y * alpha.y);
	float d = 1.0 + tan2Theta * (x + y);
	return 1.0 / (c * cos4Theta * d * d);
}

float GGXAnisoG1(vec3 alpha, vec3 normal, vec3 tangent, vec3 w)
{
	return GGXIsoG1(alpha.z, normal, w);
}

float GGXAnisoG(vec3 alpha, vec3 normal, vec3 tangent, vec3 wo, vec3 wi)
{
	return GGXAnisoG1(alpha, normal, tangent, wo) * GGXAnisoG1(alpha, normal, tangent, wi);
}

/*
	Blinn-Phong
*/

float BlinnPhongD(float alpha, vec3 normal, vec3 wh)
{
	float alpha2 = alpha * alpha;

	return 1.0 / (PI * alpha2) * pow( max(0.001, dot(normal, wh)), 2.0 / alpha2 - 2.0 );
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
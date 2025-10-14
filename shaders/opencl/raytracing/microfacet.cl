#ifndef MICROFACET_CL_
#define MICROFACET_CL_

/*
	Microfacet-related functions
*/

#include "raytracing/material_struct.cl"
#include "core/random.cl"

// EXTENDS UPON MATERIAL_XXXX. THIS IS INCLUDED IN THE MATERIAL TYPE
#define MICROFACET_BECKMANN		0x80	// Beckmann–Spizzichino model
#define MICROFACET_GGX_ISO		0x40	// Trowbridge–Reitz 'GGX' model
#define MICROFACET_GGX_ANISO	0x20	// Anisotropic version of GGX model
#define MICROFACET_BLINNPHONG	0x10	// Blinn-Phong model

static float BeckmannD(float alpha, float NdotH);
static float BeckmannG1(float alpha, float NdotW);
static float BeckmannG(float alpha,float NdotO, float NdotI);

static float GGXIsoD(float alpha, float NdotH);
static float GGXIsoG1(float alpha, float NdotW);
static float GGXIsoG(float alpha, float NdotO, float NdotI);
static float GGXIsoV(float alpha, float NdotO, float NdotI);
static float3 SampleGGX(uint* seed, float3 N, float3 T, float3 wo, float roughness);

static float GGXAnisoD(float3 alpha, float3 normal, float3 tangent, float3 wh);
static float GGXAnisoG1(float3 alpha, float3 normal, float3 tangent, float3 w);
static float GGXAnisoG(float3 alpha, float3 normal, float3 tangent, float3 wo, float3 wi);

static float BlinnPhongD(float alpha, float NdotH);
static float BlinnPhongG1(float alpha, float NdotW);
static float BlinnPhongG(float alpha, float NdotO, float NdotI);

static float3 FresnelSchlick(float cosTheta, float3 F0);
static float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness);

float3 MicrofacetBRDF(struct Material* material, float3 normal, float3 tangent, float3 wo, float3 wi)
{
	if (dot(normal, wo) == 0.f || dot(normal, wi) == 0.f) return (float3)(0.f);

	const float3 wh = normalize(wo + wi);
	const float3 F0 = mix( (float3)(0.04), material->diffuseColor, material->PBR_Metallic );

	float alpha = material->PBR_Roughness * material->PBR_Roughness;

	float NdotO = clamp(dot(normal, wo), 0.0001f, 1.f);
	float NdotI = clamp(dot(normal, wi), 0.0001f, 1.f);
	float NdotH = clamp(dot(normal, wh), 0.f, 1.f);
	float OdotH = clamp(dot(wo, wh), 0.f, 1.f);

	float DV = 0.f;
	if ((material->microfacetModel & MICROFACET_BECKMANN) != 0)
	{
		DV = BeckmannD(alpha, NdotH) * BeckmannG(alpha, NdotO, NdotI);
		DV /= 4.f * NdotO * NdotI;
	}
	if ((material->microfacetModel & MICROFACET_GGX_ISO) != 0)
	{
		DV = GGXIsoD(alpha, NdotH) * GGXIsoV(alpha, NdotO, NdotI);
	}
	if ((material->microfacetModel & MICROFACET_GGX_ANISO) != 0)
	{
		float3 anisoAlpha = (float3)(material->alphaX, material->alphaY, sqrt(material->alphaX * material->alphaY));
		DV = GGXAnisoD(anisoAlpha, normal, tangent, wh) * GGXAnisoG(anisoAlpha, normal, tangent, wo, wi);
		DV /= 4.f * NdotO * NdotI;
	}
	if ((material->microfacetModel & MICROFACET_BLINNPHONG) != 0)
	{
		DV = BlinnPhongD(alpha, NdotH) * BlinnPhongG(alpha, NdotO, NdotI);
		DV /= 4.f * NdotO * NdotI;
	}

	float3 F = FresnelSchlickRoughness(OdotH, F0, material->PBR_Roughness);
	float3 specular = (float3)(DV);

	float kD = (1.f - material->PBR_Metallic);
	float3 diffuse = kD * material->diffuseColor * M_1_PI_F;

	// Return the basic BRDF
	return mix(diffuse * NdotI, specular * NdotI, F);
}

float MicrofacetPDF(struct Material* material, float3 normal, float3 tangent, float3 wo, float3 wi)
{
	if (dot(wo, wi) < 0.f) return 0.f;

	const float3 wh = normalize(wo + wi);
	float alpha = material->PBR_Roughness * material->PBR_Roughness;

	float NdotO = clamp(dot(normal, wo), 0.0001f, 1.f);
	float NdotH = clamp(dot(normal, wh), 0.f, 1.f);
	float OdotH = clamp(dot(wo, wh), 0.f, 1.f);

	float D = 0.f;
	float G1 = 0.f;
	if ((material->microfacetModel & MICROFACET_BECKMANN) != 0)
	{
		D = BeckmannD(alpha, NdotH);
		G1 = BeckmannG1(alpha, NdotO);
	}
	if ((material->microfacetModel & MICROFACET_GGX_ISO) != 0)
	{
		D = GGXIsoD(alpha, NdotH);
		G1 = GGXIsoG1(alpha, NdotO);
	}
	if ((material->microfacetModel & MICROFACET_GGX_ANISO) != 0)
	{
		float3 anisoAlpha = (float3)(material->alphaX, material->alphaY, sqrt(material->alphaX * material->alphaY));
		D = GGXAnisoD(anisoAlpha, normal, tangent, wh);
		G1 = GGXAnisoG1(anisoAlpha, normal, tangent, wo);
	}
	if ((material->microfacetModel & MICROFACET_BLINNPHONG) != 0)
	{
		D = BlinnPhongD(alpha, NdotH);
		G1 = BlinnPhongG1(alpha, NdotO);
	}

	float PDF = G1 * OdotH * D / NdotO;
	return PDF / (4.f * OdotH);
}


float3 FresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.f - F0) * cosTheta * cosTheta * cosTheta * cosTheta * cosTheta;
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	const float x = clamp(1.0f - cosTheta, 0.f, 1.f);
	return F0 + (max((float3)(1.0f - roughness), F0) - F0) * x * x * x * x * x;
}

/****************************************************************************
 *	  				Microfacet model implementations						*
 *	https://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html	*
 ****************************************************************************/

/*
	Beckmann
*/

float BeckmannD(float alpha, float NdotH)
{
	float NdotH2 = NdotH * NdotH;
	float NdotH4 = NdotH2 * NdotH2;
	float alpha2 = alpha * alpha;
	return native_recip((M_PI_F * alpha2 * NdotH4) * exp((NdotH2 - 1.f) / (alpha2 * NdotH2)));
}

float BeckmannG1(float alpha, float NdotW)
{
	float k = alpha * M_2_SQRTPI_F;
	return (NdotW) / (NdotW * (1.f - k) + k);
}

float BeckmannG(float alpha, float NdotO, float NdotI)
{
	return BeckmannG1(alpha, NdotO) * BeckmannG1(alpha, NdotI);
}

/*
	Isotropic GGX
*/

float GGXIsoD(float alpha, float NdotH)
{
	const float a = NdotH * alpha;
	float k = alpha / (1.f - NdotH * NdotH + a * a);
	return k * k * M_1_PI_F;
}

float GGXIsoG1(float alpha, float NdotW)
{
	float k = 0.5f * alpha;
	return (NdotW) / (NdotW * (1.f - k) + k);
}

float GGXIsoG(float alpha, float NdotO, float NdotI)
{
	return GGXIsoG1(alpha, NdotO) * GGXIsoG1(alpha, NdotI);
}

float GGXIsoV(float alpha, float NdotO, float NdotI)
{
	return mix(2.f * NdotO * NdotI, NdotO + NdotI, alpha * alpha);
}

float3 SampleGGX(uint* seed, float3 N, float3 T, float3 wo, float roughness)
{
	// https://jcgt.org/published/0007/04/01/paper.pdf
	float alpha = roughness * roughness;

	// Orthonormal basis
	float3 B = normalize(cross(N, T));
	T = normalize(cross(B, N));

	// Transform view direction to local space
	float3 wo_local = (float3)(dot(wo, T), dot(wo, B), dot(wo, N));

	// Section 3.2: transforming the view direction to the hemisphere configuration
	float3 wh = normalize((float3)(alpha * wo_local.x, alpha * wo_local.y, wo_local.z));

	// Section 4.1: orthonormal basis
	float lensq = wh.x * wh.x + wh.y * wh.y;
	float3 T1 = lensq > 0.f ? (float3)(-wh.y, wh.x, 0.f) / sqrt(lensq) : (float3)(1.f, 0.f, 0.f);
	float3 T2 = cross(wh, T1);

	// Section 4.2: parameterization of the projected area
	float r1 = RandomFloat(seed);
	float r2 = RandomFloat(seed);
	float r = sqrt(r1);
	float phi = 2.f * M_PI_F * r2;
	float t1 = r * cos(phi);
	float t2 = r * sin(phi);
	float s = 0.5f * (1.f + wh.z);
	t2 = (1.f - s) * sqrt(1.f - t1 * t1) + s * t2;

	// Section 4.3: reprojection onto hemisphere
	float3 Nh = t1 * T1 + t2 * T2 + sqrt(max(0.f, 1.f - t1 * t1 - t2 * t2)) * wh;

	// Section 3.4: transforming the normal back to the ellipsoid configuration
	float3 Ne = normalize((float3)(alpha * Nh.x, alpha * Nh.y, max(0.f, Nh.z)));

	// Transform back to world space
	return normalize(Ne.x * T + Ne.y * B + Ne.z * N);
}

/*
	Anisotropic GGX
	alpha is in the form { alphaX, alhpaY, alpha }
*/

// https://pbr-book.org/4ed/Geometry_and_Transformations/Spherical_Geometry#CosPhi
float CosPhi(float3 w) {
	float sinTheta = sqrt(max(0.f, 1.f - w.y * w.y));
	return (sinTheta == 0.f) ? 1.f : clamp(w.x / sinTheta, -1.f, 1.f);
}
float SinPhi(float3 w) {
	float sinTheta = sqrt(max(0.f, 1.f - w.y * w.y));
	return (sinTheta == 0.f) ? 0.f : clamp(w.z / sinTheta, -1.f, 1.f);
}

float GGXAnisoD(float3 alpha, float3 normal, float3 tangent, float3 wh)
{
	// Convert wh to tangent space
	float3 wh_tangent = normalize((float3)(dot(wh, tangent) * alpha.x, dot(wh, normal), dot(wh, cross(tangent, normal)) * alpha.y));
	float cosTheta = wh_tangent.y;
	float cos2Theta = cosTheta * cosTheta;
	float cos4Theta = cosTheta * cosTheta * cosTheta * cosTheta;

	float sinTheta = max(0.0, 1.0 - cos2Theta);
	float tan2Theta = cos2Theta / (sinTheta * sinTheta);

	float cosPhi = CosPhi(wh_tangent);
	float sinPhi = SinPhi(wh_tangent);

	float c = M_PI_F * alpha.x * alpha.y;
	float x = (cosPhi * cosPhi) / (alpha.x * alpha.x);
	float y = (sinPhi * sinPhi) / (alpha.y * alpha.y);
	float d = 1.f + tan2Theta * (x + y);
	return 1.f / (c * cos4Theta * d * d);
}

float GGXAnisoG1(float3 alpha, float3 normal, float3 tangent, float3 w)
{
	return GGXIsoG1(alpha.z, dot(normal, w));
}

float GGXAnisoG(float3 alpha, float3 normal, float3 tangent, float3 wo, float3 wi)
{
	return GGXAnisoG1(alpha, normal, tangent, wo) * GGXAnisoG1(alpha, normal, tangent, wi);
}

/*
	Blinn-Phong
*/

float BlinnPhongD(float alpha, float NdotH)
{
	float alpha2 = alpha * alpha;

	return 1.0 / (M_PI_F * alpha2) * pow( NdotH, 2.f / alpha2 - 2.f );
}

float BlinnPhongG1(float alpha, float NdotW)
{
	// The Smith integral has no closed form solution for Blinn-Phong. Walter [4] suggests using the same equation as Beckmann.
	return BeckmannG1(alpha, NdotW);
}

float BlinnPhongG(float alpha, float NdotO, float NdotI)
{
	return BlinnPhongG1(alpha, NdotO) * BlinnPhongG1(alpha, NdotI);
}

#endif

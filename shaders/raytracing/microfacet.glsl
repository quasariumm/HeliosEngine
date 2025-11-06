#ifndef MICROFACET_GLSL
#define MICROFACET_GLSL

/*
	Houses the material struct and material-related functions
*/

#include "/Engine/raytracing/materialStruct.glsl"

// EXTENDS UPON MATERIAL_XXXX. THIS IS INCLUDED IN THE MATERIAL TYPE
#define MICROFACET_BECKMANN		0x100	// Beckmann–Spizzichino model
#define MICROFACET_GGX_ISO		0x200	// Trowbridge–Reitz 'GGX' model
#define MICROFACET_GGX_ANISO	0x400	// Anisotropic version of GGX model
#define MICROFACET_BLINNPHONG	0x800	// Blinn-Phong model

float BeckmannD(float alpha, float NdotH);
float BeckmannG1(float alpha, float NdotW);
float BeckmannG(float alpha,float NdotO, float NdotI);

float GGXIsoD(float alpha, float NdotH);
float GGXIsoG1(float alpha, float NdotW);
float GGXIsoG(float alpha, float NdotO, float NdotI);
float GGXIsoV(float alpha, float NdotO, float NdotI);
vec3 SampleGGX(inout uint seed, vec3 N, vec3 T, vec3 wo, float roughness);

float GGXAnisoD(vec3 alpha, vec3 normal, vec3 tangent, vec3 wh);
float GGXAnisoG1(vec3 alpha, vec3 normal, vec3 tangent, vec3 w);
float GGXAnisoG(vec3 alpha, vec3 normal, vec3 tangent, vec3 wo, vec3 wi);

float BlinnPhongD(float alpha, float NdotH);
float BlinnPhongG1(float alpha, float NdotW);
float BlinnPhongG(float alpha, float NdotO, float NdotI);

vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

vec3 MicrofacetBRDF(RayTracingMaterial material, vec3 normal, vec3 tangent, vec3 wo, vec3 wi)
{
	if (dot(normal, wo) == 0.0 || dot(normal, wi) == 0.0) return vec3(0.0);

	const vec3 wh = normalize(wo + wi);
	const vec3 F0 = mix( vec3(0.04), material.diffuseColor, material.PBR_Metallic );

	float alpha = material.PBR_Roughness * material.PBR_Roughness;

	float NdotO = clamp(dot(normal, wo), 0.0001, 1.0);
	float NdotI = clamp(dot(normal, wi), 0.0001, 1.0);
	float NdotH = clamp(dot(normal, wh), 0.0, 1.0);
	float OdotH = clamp(dot(wo, wh), 0.0, 1.0);

	float DV = 0.0;
	if ((material.materialProperties & MICROFACET_BECKMANN) != 0)
	{
		DV = BeckmannD(alpha, NdotH) * BeckmannG(alpha, NdotO, NdotI);
		DV /= 4.0 * NdotO * NdotI;
	}
	if ((material.materialProperties & MICROFACET_GGX_ISO) != 0)
	{
		DV = GGXIsoD(alpha, NdotH) * GGXIsoV(alpha, NdotO, NdotI);
	}
	if ((material.materialProperties & MICROFACET_GGX_ANISO) != 0)
	{
		vec3 anisoAlpha = vec3(material.alphaX, material.alphaY, sqrt(material.alphaX * material.alphaY));
		DV = GGXAnisoD(anisoAlpha, normal, tangent, wh) * GGXAnisoG(anisoAlpha, normal, tangent, wo, wi);
		DV /= 4.0 * NdotO * NdotI;
	}
	if ((material.materialProperties & MICROFACET_BLINNPHONG) != 0)
	{
		DV = BlinnPhongD(alpha, NdotH) * BlinnPhongG(alpha, NdotO, NdotI);
		DV /= 4.0 * NdotO * NdotI;
	}

	vec3 F = FresnelSchlickRoughness(OdotH, F0, material.PBR_Roughness);
	vec3 specular = vec3(DV);

	float kD = (1.f - material.PBR_Metallic);
	vec3 diffuse = kD * material.diffuseColor * INVPI;

	// Return the basic BRDF
	return mix(diffuse * NdotI, specular * NdotI, F);
}

float MicrofacetPDF(RayTracingMaterial material, vec3 normal, vec3 tangent, vec3 wo, vec3 wi)
{
	if (dot(wo, wi) < 0.0) return 0.0;

	const vec3 wh = normalize(wo + wi);
	float alpha = material.PBR_Roughness * material.PBR_Roughness;

	float NdotO = clamp(dot(normal, wo), 0.0001, 1.0);
	float NdotH = clamp(dot(normal, wh), 0.0, 1.0);
	float OdotH = clamp(dot(wo, wh), 0.0, 1.0);

	float D = 0.0;
	float G1 = 0.0;
	if ((material.materialProperties & MICROFACET_BECKMANN) != 0)
	{
		D = BeckmannD(alpha, NdotH);
		G1 = BeckmannG1(alpha, NdotO);
	}
	if ((material.materialProperties & MICROFACET_GGX_ISO) != 0)
	{
		D = GGXIsoD(alpha, NdotH);
		G1 = GGXIsoG1(alpha, NdotO);
	}
	if ((material.materialProperties & MICROFACET_GGX_ANISO) != 0)
	{
		vec3 anisoAlpha = vec3(material.alphaX, material.alphaY, sqrt(material.alphaX * material.alphaY));
		D = GGXAnisoD(anisoAlpha, normal, tangent, wh);
		G1 = GGXAnisoG1(anisoAlpha, normal, tangent, wo);
	}
	if ((material.materialProperties & MICROFACET_BLINNPHONG) != 0)
	{
		D = BlinnPhongD(alpha, NdotH);
		G1 = BlinnPhongG1(alpha, NdotO);
	}

	float PDF = G1 * OdotH * D / NdotO;
	return PDF / (4.0 * OdotH);
}


vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * cosTheta * cosTheta * cosTheta * cosTheta * cosTheta;
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	const float x = clamp(1.0f - cosTheta, 0.0, 1.0);
	return F0 + (max(vec3(1.0f - roughness), F0) - F0) * x * x * x * x * x;
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
	return 1.0 / (PI * alpha2 * NdotH4) * exp((NdotH2 - 1.0) / (alpha2 * NdotH2));
}

float BeckmannG1(float alpha, float NdotW)
{
	float k = alpha * SQRT_TWO_OVER_PI;
	return (NdotW) / (NdotW * (1.0 - k) + k);
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
	return k * k * (1.f / PI);
}

float GGXIsoG1(float alpha, float NdotW)
{
	float k = 0.5 * alpha;
	return (NdotW) / (NdotW * (1.0 - k) + k);
}

float GGXIsoG(float alpha, float NdotO, float NdotI)
{
	return GGXIsoG1(alpha, NdotO) * GGXIsoG1(alpha, NdotI);
}

float GGXIsoV(float alpha, float NdotO, float NdotI)
{
	return mix(2.f * NdotO * NdotI, NdotO + NdotI, alpha * alpha);
}

vec3 SampleGGX(inout uint seed, vec3 N, vec3 T, vec3 wo, float roughness)
{
	// https://jcgt.org/published/0007/04/01/paper.pdf
	float alpha = roughness * roughness;

	// Orthonormal basis
	vec3 B = normalize(cross(N, T));
	T = normalize(cross(B, N));

	// Transform view direction to local space
	vec3 wo_local = vec3(dot(wo, T), dot(wo, B), dot(wo, N));

	// Section 3.2: transforming the view direction to the hemisphere configuration
	vec3 wh = normalize(vec3(alpha * wo_local.x, alpha * wo_local.y, wo_local.z));

	// Section 4.1: orthonormal basis
	float lensq = wh.x * wh.x + wh.y * wh.y;
	vec3 T1 = lensq > 0.0 ? vec3(-wh.y, wh.x, 0.0) / sqrt(lensq) : vec3(1.0, 0.0, 0.0);
	vec3 T2 = cross(wh, T1);

	// Section 4.2: parameterization of the projected area
	float r1 = RandomFloat(seed);
	float r2 = RandomFloat(seed);
	float r = sqrt(r1);
	float phi = 2.0 * PI * r2;
	float t1 = r * cos(phi);
	float t2 = r * sin(phi);
	float s = 0.5 * (1.0 + wh.z);
	t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;

	// Section 4.3: reprojection onto hemisphere
	vec3 Nh = t1 * T1 + t2 * T2 + sqrt(max(0.0, 1.0 - t1 * t1 - t2 * t2)) * wh;

	// Section 3.4: transforming the normal back to the ellipsoid configuration
	vec3 Ne = normalize(vec3(alpha * Nh.x, alpha * Nh.y, max(0.0, Nh.z)));

	// Transform back to world space
	return normalize(Ne.x * T + Ne.y * B + Ne.z * N);
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
	return GGXIsoG1(alpha.z, dot(normal, w));
}

float GGXAnisoG(vec3 alpha, vec3 normal, vec3 tangent, vec3 wo, vec3 wi)
{
	return GGXAnisoG1(alpha, normal, tangent, wo) * GGXAnisoG1(alpha, normal, tangent, wi);
}

/*
	Blinn-Phong
*/

float BlinnPhongD(float alpha, float NdotH)
{
	float alpha2 = alpha * alpha;

	return 1.0 / (PI * alpha2) * pow( NdotH, 2.0 / alpha2 - 2.0 );
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


#endif // MICROFACET_GLSL
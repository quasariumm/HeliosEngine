#ifndef MATERIALS_CL_
#define MATERIALS_CL_

#include "raytracing/ray.cl"
#include "raytracing/microfacet.cl"

/*
	Bounces
*/

inline float3 LambertianBounce(uint* seed, float3 normal)
{
	return normalize(normal + RandomHemisphericalDirection(seed));
}

inline float3 Reflect(float3 wo, float3 normal)
{
	return normalize(-wo + 2.f * dot(wo, normal) * normal);
}

// Returns direction in xyz, Fresnel term in w
// TODO: Support for refraction from non-air to non-air
static float4 Refract(float3 wo, float3 normal, float etaI, float etaO)
{
	float eta = etaI / etaO;
	float3 N = normal;
	float cosThetaI = dot(N, wo);

	float sin2ThetaI = max(0.0001f, 1.f - (cosThetaI * cosThetaI));

	float sin2ThetaT = sin2ThetaI / (eta * eta);
	if (sin2ThetaT >= 1.f)
		return (float4)(1.f); // TIR

	float cosThetaT = sqrt(1.f - sin2ThetaT);

	float R0 = (etaI - etaO) / (etaI + etaO);
	R0 *= R0;
	float x = (1.f - cosThetaI);
	float fresnel = R0 + (1.f - R0) * x * x * x * x * x;

	return (float4)(normalize(-wo / eta + (cosThetaI / eta - cosThetaT) * N), fresnel);
}

/*
	BRDF, BSSDF and PDF
*/

struct BRDFInfo
{
	float factor;
	float3 direction;
	float3 BRDF;
};

// These functions are combined for the reason that the same values need to be calculated eitherway
static float3 GetBRDFAndBounce(struct Ray* ray, struct Material* material, uint* seed)
{
	struct BRDFInfo diffuse 		= {0.f, (float3)(0.f), (float3)(0.f)};
	struct BRDFInfo specular 		= {0.f, (float3)(0.f), (float3)(0.f)};
	struct BRDFInfo transmission 	= {0.f, (float3)(0.f), (float3)(0.f)};

	bool glassReflect = false;

	if ((material->properties & MATERIAL_MICROFACET) != 0)
	{
		// Direction: roughness affects the reflection
		if ((material->properties & MATERIAL_REFLECTION) != 0)
		{
			if (material->PBR_Roughness < 0.001)
			{
				// Perfect mirror reflection
				specular.direction = Reflect(-ray->dir, ray->hit.normal);
			}
			else
			{
				// Rough reflection: sample GGX distribution
				float3 wh = SampleGGX(seed, ray->hit.normal, ray->hit.tangent, -ray->dir, material->PBR_Roughness);
				specular.direction = Reflect(-ray->dir, wh);
			}
		}

		specular.factor = 1.f;
		specular.BRDF = MicrofacetBRDF(material, ray->hit.normal, ray->hit.tangent, -ray->dir, ray->hit.lightVector);
	}
	else
	{
		if ((material->properties & MATERIAL_DIFFUSE) != 0)
		{
			// Direction
			if ((material->properties & MATERIAL_REFLECTION) != 0)
				diffuse.direction = LambertianBounce(seed, ray->hit.normal);
			// Factor
			diffuse.factor = 1.f - material->specularity;
			// BRDF
			diffuse.BRDF = material->diffuseColor * M_1_PI_F;
		}
		if ((material->properties & MATERIAL_SPECULAR) != 0)
		{
			// Direction
			if ((material->properties & MATERIAL_REFLECTION) != 0)
				specular.direction = Reflect(-ray->dir, ray->hit.normal);

			specular.factor = material->specularity;
			// BRDF
			// https://www.cs.cmu.edu/afs/cs/academic/class/15462-f09/www/lec/lec8.pdf
			float3 H = normalize(ray->hit.lightVector - ray->dir);
			float overlap = max(0.001f, dot(H, ray->hit.normal));
			float k = (material->shininess + 2.f) * 0.5f * M_1_PI_F;
			specular.BRDF = k * material->specularColor * pow(overlap, material->shininess);
		}
		if ((material->properties & MATERIAL_TRANSMISSION) != 0
		    && material->refractivity > 0.f)
		{
			transmission.factor = material->refractivity;

			if ((material->properties & MATERIAL_REFLECTION) != 0)
			{
				float etaI = ray->hit.inside ? material->refractionCoefficient : 1.f;
				float etaO = ray->hit.inside ? 1.f : material->refractionCoefficient;
				// Direction
				float4 res = Refract(-ray->dir, ray->hit.normal, etaI, etaO);
				float fresnel = res.w;
				if (Xi(seed) <= mix(1.f, fresnel, transmission.factor))
					glassReflect = true;
				specular.factor *= (float)(glassReflect);

				transmission.direction = res.xyz;
				// BRDF
				if ((material->properties & MATERIAL_MICROFACET) != 0)
				{
					transmission.BRDF = MicrofacetBRDF(material, ray->hit.normal, ray->hit.tangent, -ray->dir, ray->hit.lightVector);
				}
				else
				{
					// https://cgg.mff.cuni.cz/~jaroslav/teaching/2017-npgr010/slides/03%20-%20npgr010-2017%20-%20BRDF.pdf
					float overlap = max(0.001f, -dot(res.xyz, ray->dir));
					float changeOfRadiance = (etaO * etaO) / (etaI * etaI);
					// Physically-plausible Phong Distributuion
					float3 DPPPhong = material->specularColor * ((material->shininess + 2.f) / (2.f * M_PI_F)) * pow(overlap, material->shininess);
					transmission.BRDF = changeOfRadiance * (1.f - fresnel) * DPPPhong / dot(ray->hit.normal, ray->hit.lightVector);
					transmission.factor *= (float)(!glassReflect);
				}
			}
		}
	}

	if ((material->properties & MATERIAL_MICROFACET) != 0 && (material->properties & MICROFACET_GGX_ANISO) != 0)
	{
		float3 wh = normalize(-ray->dir + ray->hit.lightVector);
		float3 wh_tangent = normalize((float3)(
			dot(wh, ray->hit.tangent) * material->alphaX,
			dot(wh, ray->hit.normal),
			dot(wh, cross(ray->hit.tangent, ray->hit.normal)) * material->alphaY
		));
		wh = wh_tangent.x * ray->hit.tangent + wh_tangent.y * ray->hit.normal + wh_tangent.z * cross(ray->hit.tangent, ray->hit.normal);
		float3 reflected = Reflect(-ray->dir, wh);
		ray->dir = diffuse.direction * diffuse.factor + reflected * specular.factor;
	}
	else
	{
		ray->dir = diffuse.direction * diffuse.factor + specular.direction * specular.factor + transmission.direction * transmission.factor;
	}
	return diffuse.BRDF * diffuse.factor + specular.BRDF * specular.factor + transmission.BRDF * transmission.factor;
}

struct PDFInfo
{
	float factor;
	float PDF;
};

static float GetPDF(struct Material* material, float3 normal, float3 tangent, float3 wo, float3 wi)
{
	struct PDFInfo diffuse = {
		1.f - material->specularity,
		dot(normal, wo) * 0.5f * M_1_PI_F
    };
	struct PDFInfo specularTransmission = {
		material->specularity,
		1.f
    };

	if ((material->properties & MATERIAL_MICROFACET) != 0)
	{
		specularTransmission.factor = material->PBR_Metallic;
		diffuse.factor = 1.f - specularTransmission.factor;

		specularTransmission.PDF = MicrofacetPDF(material, normal, tangent, wo, wi);
	}

	return diffuse.PDF * diffuse.factor + specularTransmission.PDF * specularTransmission.factor;
}

#endif

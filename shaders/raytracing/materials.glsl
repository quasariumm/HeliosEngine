#ifndef MATERIALS_GLSL
#define MATERIALS_GLSL

#include "/Engine/core/random.glsl"
#include "/Engine/raytracing/ray.glsl"
#include "/Engine/raytracing/materialStruct.glsl"
#include "/Engine/raytracing/microfacet.glsl"

/*
	Bounces
*/

vec3 LambertianBounce(inout uint seed, vec3 normal)
{
	return normalize(normal + RandomHemisphericalDirection(seed));
}

vec3 Reflect(vec3 wo, vec3 normal)
{
	return normalize(-wo + 2.0 * dot(wo, normal) * normal);
}

// Returns direction in xyz, Fresnel term in w
// TODO: Support for refraction from non-air to non-air
vec4 Refract(vec3 wo, vec3 normal, float etaI, float etaO)
{
	float eta = etaI / etaO;
	vec3 N = normal;
	float cosThetaI = dot(N, wo);

	float sin2ThetaI = max(0.0001, 1.0 - (cosThetaI * cosThetaI));

	float sin2ThetaT = sin2ThetaI / (eta * eta);
	if (sin2ThetaT >= 1.0)
		return vec4(1.0); // TIR

	float cosThetaT = sqrt(1.0 - sin2ThetaT);

	float R0 = (etaI - etaO) / (etaI + etaO);
	R0 *= R0;
	float x = (1.0 - cosThetaI);
	float fresnel = R0 + (1.0 - R0) * x * x * x * x * x;

	return vec4(normalize(-wo / eta + (cosThetaI / eta - cosThetaT) * N), fresnel);
}

/*
	BRDF, BSSDF and PDF
*/

struct BRDFInfo
{
	float factor;
	vec3 direction;
	vec3 BRDF;
};

// These functions are combined for the reason that the same values need to be calculated eitherway
vec3 GetBRDFAndBounce(inout Ray ray, inout uint seed)
{
	BRDFInfo diffuse 		= BRDFInfo(0.0, vec3(0.0), vec3(0.0));
	BRDFInfo specular 		= BRDFInfo(0.0, vec3(0.0), vec3(0.0));
	BRDFInfo transmission 	= BRDFInfo(0.0, vec3(0.0), vec3(0.0));

	bool glassReflect = false;

	RayTracingMaterial material = Materials[ray.hit.materialIndex];

	if ((material.materialProperties & MATERIAL_MICROFACET) != 0)
	{
		// Direction: roughness affects the reflection
		if ((material.materialProperties & MATERIAL_REFLECTION) != 0)
		{
			if (material.PBR_Roughness < 0.001)
			{
				// Perfect mirror reflection
				specular.direction = Reflect(-ray.dir, ray.hit.normal);
			}
			else
			{
				// Rough reflection: sample GGX distribution
				vec3 wh = SampleGGX(seed, ray.hit.normal, ray.hit.tangent, -ray.dir, material.PBR_Roughness);
				specular.direction = Reflect(-ray.dir, wh);
			}
		}

		specular.factor = 1.f;
		specular.BRDF = MicrofacetBRDF(material, ray.hit.normal, ray.hit.tangent, -ray.dir, ray.hit.lightVector);
	}
	else
	{
		if ((material.materialProperties & MATERIAL_DIFFUSE) != 0)
		{
			// Direction
			if ((material.materialProperties & MATERIAL_REFLECTION) != 0)
				diffuse.direction = LambertianBounce(seed, ray.hit.normal);
			// Factor
			diffuse.factor = 1.0 - material.specularity;
			// BRDF
			diffuse.BRDF = material.diffuseColor * INVPI;
		}
		if ((material.materialProperties & MATERIAL_SPECULAR) != 0)
		{
			// Direction
			if ((material.materialProperties & MATERIAL_REFLECTION) != 0)
				specular.direction = Reflect(-ray.dir, ray.hit.normal);

			specular.factor = material.specularity;
			// BRDF
			// https://www.cs.cmu.edu/afs/cs/academic/class/15462-f09/www/lec/lec8.pdf
			vec3 H = normalize(ray.hit.lightVector - ray.dir);
			float overlap = max(0.001, dot(H, ray.hit.normal));
			float k = (material.shininess + 2.0) * INV2PI;
			specular.BRDF = k * material.specularColor * pow(overlap, material.shininess);
		}
		if ((material.materialProperties & MATERIAL_TRANSMISSION) != 0
		&& material.refractivity > 0.0)
		{
			transmission.factor = material.refractivity;

			if ((material.materialProperties & MATERIAL_REFLECTION) != 0)
			{
				float etaI = ray.hit.inside ? material.refractionCoefficient : 1.0;
				float etaO = ray.hit.inside ? 1.0 : material.refractionCoefficient;
				// Direction
				vec4 res = Refract(-ray.dir, ray.hit.normal, etaI, etaO);
				float fresnel = res.w;
				if (Xi(seed) <= mix(1.0, fresnel, transmission.factor))
					glassReflect = true;
				specular.factor *= float(glassReflect);

				transmission.direction = res.xyz;
				// BRDF
				if ((material.materialProperties & MATERIAL_MICROFACET) != 0)
				{
					transmission.BRDF = MicrofacetBRDF(material, ray.hit.normal, ray.hit.tangent, -ray.dir, ray.hit.lightVector);
				}
				else
				{
					// https://cgg.mff.cuni.cz/~jaroslav/teaching/2017-npgr010/slides/03%20-%20npgr010-2017%20-%20BRDF.pdf
					float overlap = max(0.001, -dot(res.xyz, ray.dir));
					float changeOfRadiance = (etaO * etaO) / (etaI * etaI);
					// Physically-plausible Phong Distributuion
					vec3 DPPPhong = material.specularColor * ((material.shininess + 2.0) / TWO_PI) * pow(overlap, material.shininess);
					transmission.BRDF = changeOfRadiance * (1.0 - fresnel) * DPPPhong / dot(ray.hit.normal, ray.hit.lightVector);
					transmission.factor *= float(!glassReflect);
				}
			}
		}
	}

	if ((material.materialProperties & MATERIAL_MICROFACET) != 0 && (material.materialProperties & MICROFACET_GGX_ANISO) != 0)
	{
		vec3 wh = normalize(-ray.dir + ray.hit.lightVector);
		vec3 bitangent = cross(ray.hit.tangent.xyz, ray.hit.normal) * ray.hit.tangent.w;
		vec3 wh_tangent = normalize(vec3(
			dot(wh, ray.hit.tangent.xyz) * material.alphaX,
			dot(wh, ray.hit.normal),
			dot(wh, bitangent) * material.alphaY
		));
		wh = wh_tangent.x * ray.hit.tangent.xyz + wh_tangent.y * ray.hit.normal + wh_tangent.z * bitangent;
		vec3 reflected = Reflect(-ray.dir, wh);
		ray.dir = diffuse.direction * diffuse.factor + reflected * specular.factor;
	}
	else
	{
		ray.dir = diffuse.direction * diffuse.factor + specular.direction * specular.factor + transmission.direction * transmission.factor;
	}
	return diffuse.BRDF * diffuse.factor + specular.BRDF * specular.factor + transmission.BRDF * transmission.factor;
}

struct PDFInfo
{
	float factor;
	float PDF;
};

float GetPDF(RayTracingMaterial material, vec3 normal, vec4 tangent, vec3 wo, vec3 wi)
{
	PDFInfo diffuse = PDFInfo(
		1.0 - material.specularity,
		dot(normal, wo) * INV2PI
	);
	PDFInfo specularTransmission = PDFInfo(
		material.specularity,
		1.0
	);

	if ((material.materialProperties & MATERIAL_MICROFACET) != 0)
	{
		specularTransmission.factor = material.PBR_Metallic;
		diffuse.factor = 1.0 - specularTransmission.factor;

		specularTransmission.PDF = MicrofacetPDF(material, normal, tangent, wo, wi);
	}

	return diffuse.PDF * diffuse.factor + specularTransmission.PDF * specularTransmission.factor;
}

#endif // MATERIALS_GLSL
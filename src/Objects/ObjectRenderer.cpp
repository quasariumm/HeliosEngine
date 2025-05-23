#include "ObjectRenderer.h"

#include "Components/Material.h"


namespace Engine
{

std::vector<RenderObject> ObjectRenderer::m_renderObjects = {};
GL46_ComputeShader* ObjectRenderer::m_computeShader = {};

void ObjectRenderer::SendObjectData()
{
    if (m_computeShader == nullptr) return;
    m_computeShader->Use();

    int sphereIdx = 0;
    int cubeIdx = 0;
    int modelIdx = 0;

    for (const RenderObject& object : m_renderObjects)
    {
        std::string baseName;

        switch (object.primitiveType)
        {
        case PrimitiveType::SPHERE:
            SetSphereData(sphereIdx, object.transform->position(), *object.radius );
            baseName = "Spheres[" + std::to_string(sphereIdx) + "]";
            SetMaterialData(baseName, *object.materialIdx);
            sphereIdx++;
            break;
        case PrimitiveType::CUBE:
            cubeIdx++;
            break;
        case PrimitiveType::MODEL:
            modelIdx++;
            break;
        }

    }

     m_computeShader->SetInt("NumSpheres", sphereIdx);
}

void ObjectRenderer::RegisterModelInstance(Transform* transform, uint32_t modelUID)
{
    m_renderObjects.emplace_back(PrimitiveType::MODEL, transform, modelUID);
}

void ObjectRenderer::RegisterSphere(Transform* transform, float* radius, int* materialIdx)
{
    m_renderObjects.emplace_back(PrimitiveType::SPHERE, transform, -1, radius, materialIdx);
}

void ObjectRenderer::SetSphereData(const int idx, const vec3 position, const float radius)
{
    if (m_computeShader == nullptr) return;
    const std::string baseName = "Spheres[" + std::to_string(idx) + "]";
    m_computeShader->SetVec3(baseName + ".center", position);
    m_computeShader->SetFloat(baseName + ".radius", radius);

}

void ObjectRenderer::SetMaterialData(const std::string& base, const int materialIdx)
{
    if (m_computeShader == nullptr) return;
    const std::string matBaseName = base + ".material";
	if (materialIdx < 0 || materialIdx >= materials.size())
		throw std::range_error("Material index out of range");
	const Material* mat = materials[materialIdx];

	const int type = mat->m_properties.reflection
		| (mat->m_properties.microfacet << 1)
		| (mat->m_properties.transmission << 2)
		| (mat->m_properties.diffuse << 3)
		| (mat->m_properties.glossy << 4)
		| (mat->m_properties.specular << 5)
		| (mat->m_microfacetModel.beckmann << 6)
		| (mat->m_microfacetModel.ggx_iso << 7)
		| (mat->m_microfacetModel.ggx_aniso << 8)
		| (mat->m_microfacetModel.blinnphong << 9);
	m_computeShader->SetInt(matBaseName + ".type", type);

    m_computeShader->SetVec3(matBaseName + ".diffuseColor", mat->m_diffuseColor);
    m_computeShader->SetVec3(matBaseName + ".specularColor", mat->m_specularColor);
    m_computeShader->SetFloat(matBaseName + ".specularity", mat->m_specularity);
    m_computeShader->SetFloat(matBaseName + ".shininess", mat->m_shininess);
    m_computeShader->SetFloat(matBaseName + ".specularProbability", mat->m_glossiness);

    m_computeShader->SetVec3(matBaseName + ".emissionColor", mat->m_emissionColor);
    m_computeShader->SetFloat(matBaseName + ".emissionStrength", mat->m_emissionStrength);

    m_computeShader->SetFloat(matBaseName + ".refractivity", mat->m_refractivity);
    m_computeShader->SetFloat(matBaseName + ".refractionCoefficient", mat->m_refractionCoefficient);

	m_computeShader->SetFloat(matBaseName + ".PBR_Roughness", mat->m_PBR_Roughness);
	m_computeShader->SetFloat(matBaseName + ".PBR_Metallic", mat->m_PBR_Metallic);
	m_computeShader->SetFloat(matBaseName + ".PBR_Reflectance", mat->m_PBR_Reflectance);
}

}

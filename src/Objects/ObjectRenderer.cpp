#include "ObjectRenderer.h"

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
            SetMaterialData(baseName);
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

void ObjectRenderer::RegisterSphere(Transform* transform, float* radius)
{
    m_renderObjects.emplace_back(PrimitiveType::SPHERE, transform, -1, radius);
}

void ObjectRenderer::SetSphereData(const int idx, const vec3 position, const float radius)
{
    if (m_computeShader == nullptr) return;
    const std::string baseName = "Spheres[" + std::to_string(idx) + "]";
    m_computeShader->SetVec3(baseName + ".center", position);
    m_computeShader->SetFloat(baseName + ".radius", radius);

}

void ObjectRenderer::SetMaterialData(const std::string& base)
{
    if (m_computeShader == nullptr) return;
    const std::string matBaseName = base + ".material";
	m_computeShader->SetInt(matBaseName + ".type", 0b10111111);
    m_computeShader->SetVec3(matBaseName + ".diffuseColor", vec3f(0.9f, 0.6f, 0.3f));
    m_computeShader->SetVec3(matBaseName + ".specularColor", vec3f(1.f));
    m_computeShader->SetFloat(matBaseName + ".specularity", (base == "Spheres[1]" || base == "Spheres[2]") ? 1.0f : 0.f);
    m_computeShader->SetFloat(matBaseName + ".shininess", (base == "Spheres[1]" || base == "Spheres[2]") ? 20.0f : 0.f);
    m_computeShader->SetFloat(matBaseName + ".specularProbability", 1.f);

    m_computeShader->SetVec3(matBaseName + ".emissionColor", vec3f(1.f));
    m_computeShader->SetFloat(matBaseName + ".emissionStrength", 0.f);

    m_computeShader->SetFloat(matBaseName + ".refractivity", (base == "Spheres[2]") ? 1.0f : 0.f);
    m_computeShader->SetFloat(matBaseName + ".refractionCoefficient", 1.52f);

	m_computeShader->SetFloat(matBaseName + ".PBR_Roughness", 0.6f);
	m_computeShader->SetFloat(matBaseName + ".PBR_Metallic", 0.3f);
	m_computeShader->SetFloat(matBaseName + ".PBR_Reflectance", 0.3f);
}

}

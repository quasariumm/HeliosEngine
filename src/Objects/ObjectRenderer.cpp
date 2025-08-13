#include "ObjectRenderer.h"

#include "Assets/ModelFileHandler.h"
#include "Components/Light.h"
#include "Components/Material.h"


namespace Engine
{


ObjectRenderer::~ObjectRenderer()
{
	glDeleteBuffers(1, &m_meshSSBO);
	glDeleteBuffers(1, &m_vertexSSBO);
	glDeleteBuffers(1, &m_indexSSBO);
	glDeleteBuffers(1, &m_materialSSBO);
}


void ObjectRenderer::SetShader(GL46_ComputeShader* shader)
{
	m_computeShader = shader;
}


void ObjectRenderer::SendObjectData()
{
    if (m_computeShader == nullptr) return;
    m_computeShader->Use();

    int sphereIdx = 0;
    int cubeIdx = 0;

    for (const RenderObject& object : m_renderObjects)
    {
        std::string baseName;

        switch (object.primitiveType)
        {
        case PrimitiveType::SPHERE:
            SetSphereData(sphereIdx, object.transform->position(), *object.radius, *object.materialIdx );
            baseName = "Spheres[" + std::to_string(sphereIdx) + "]";
            sphereIdx++;
            break;
        case PrimitiveType::CUBE:
            cubeIdx++;
            break;
        case PrimitiveType::MODEL:
            break;
        }

    }

     m_computeShader->SetInt("NumSpheres", sphereIdx);

	uint8_t pointIdx = 0;
	uint8_t directionalIdx = 0;
	uint8_t simpleSpotIdx = 0;
	uint8_t iesIdx = 0;

	for (const Light* light : lights)
	{
		std::string baseName;

		switch (light->lightType)
		{
		case LightType::POINT:
			baseName = "PointLights[" + std::to_string(pointIdx) + "]";
			m_computeShader->SetVec3(baseName + ".position", light->position);
			m_computeShader->SetVec3(baseName + ".color", light->color);
			m_computeShader->SetFloat(baseName + ".intensity", light->intensity);
			pointIdx++;
			break;
		case LightType::DIRECTIONAL:
			baseName = "DirectionalLights[" + std::to_string(pointIdx) + "]";
			m_computeShader->SetVec3(baseName + ".color", light->color);
			m_computeShader->SetVec3(baseName + ".direction", light->direction);
			m_computeShader->SetFloat(baseName + ".intensity", light->intensity);
			directionalIdx++;
			break;
		case LightType::SIMPLE_SPOT:
			baseName = "SimpleSpotLights[" + std::to_string(pointIdx) + "]";
			m_computeShader->SetVec3(baseName + ".position", light->position);
			m_computeShader->SetVec3(baseName + ".color", light->color);
			m_computeShader->SetVec3(baseName + ".direction", light->direction);
			m_computeShader->SetFloat(baseName + ".intensity", light->intensity);
			m_computeShader->SetFloat(baseName + ".innerCutOff", light->innerCutOff);
			m_computeShader->SetFloat(baseName + ".outerCutOff", light->outerCutOff);
			simpleSpotIdx++;
			break;
		case LightType::IES_SPOT:
			// TODO
			iesIdx++;
			break;
		default:
			break;
		}
	}

	m_computeShader->SetUInt("NumPointLights", pointIdx);
	m_computeShader->SetUInt("NumDirectionalLights", directionalIdx);
	m_computeShader->SetUInt("NumSimpleSpotLights", simpleSpotIdx);
}

void ObjectRenderer::RegisterModelInstance(Transform* transform, ModelData** modelDataLoc)
{
    m_renderObjects.emplace_back(PrimitiveType::MODEL, transform, modelDataLoc);
	UpdateModelSSBOs();
}

void ObjectRenderer::RegisterSphere(Transform* transform, float* radius, int* materialIdx)
{
    m_renderObjects.emplace_back(PrimitiveType::SPHERE, transform, nullptr, radius, materialIdx);
}


void ObjectRenderer::UpdateModelSSBOs()
{
	if (m_computeShader == nullptr) return;
	// Delete old buffers
	glDeleteBuffers(1, &m_meshSSBO);
	glDeleteBuffers(1, &m_vertexSSBO);
	glDeleteBuffers(1, &m_indexSSBO);

	// Generate new ones that we're going to use
	glGenBuffers(1, &m_meshSSBO);
	glGenBuffers(1, &m_vertexSSBO);
	glGenBuffers(1, &m_indexSSBO);

	// Associate the buffers with the GL_SHADER_STORAGE_BUFFER name
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_meshSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_vertexSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_indexSSBO);

	// Determine the amount of vertices, indices and meshes
	uint32_t meshAmount = 0;
	uint32_t vertexAmount = 0;
	uint32_t indexAmount = 0;
	for (const RenderObject& renderObject : m_renderObjects)
	{
		if (renderObject.primitiveType != PrimitiveType::MODEL) continue;
		ModelData* modelData = *renderObject.modelDataLoc;
		if (modelData == nullptr) continue;
		meshAmount += modelData->meshes.size();
		for (MeshData& mesh : modelData->meshes)
		{
			vertexAmount += mesh.vertices.size();
			indexAmount += mesh.indices.size();
		}
	}

	// Set the Num**** variables in the first part of the data
	m_computeShader->Use();
	m_computeShader->SetUInt("NumMeshes", meshAmount);
	m_computeShader->SetUInt("NumVertices", vertexAmount);
	m_computeShader->SetUInt("NumIndices", indexAmount);

	if (meshAmount == 0) return;

	// Allocate enough space for the buffers
	glNamedBufferStorage(
		m_meshSSBO,
		sizeofll(GPUMesh) * meshAmount,
		nullptr,
		GL_DYNAMIC_STORAGE_BIT
	);
	glNamedBufferStorage(
		m_vertexSSBO,
		sizeofll(VertexData) * vertexAmount,
		nullptr,
		GL_DYNAMIC_STORAGE_BIT
	);
	glNamedBufferStorage(
		m_indexSSBO,
		sizeofll(uint32_t) * indexAmount,
		nullptr,
		GL_DYNAMIC_STORAGE_BIT
	);

	// Send data per mesh
	uint32_t meshIndex = 0;
	uint32_t vertexIndex = 0;
	uint32_t indexIndex = 0;

	const int numMaterials = MaterialRegister::Instance().GetNumMaterials();

	for (const RenderObject& renderObject : m_renderObjects)
	{
		if (renderObject.primitiveType != PrimitiveType::MODEL) continue;
		const ModelData* modelData = *renderObject.modelDataLoc;
		if (modelData == nullptr) continue;
		for (const MeshData& mesh : modelData->meshes)
		{
			const int materialIndex = (mesh.materialIndex >= 0 && mesh.materialIndex < numMaterials) ? mesh.materialIndex + 1 : 0;
			GPUMesh gpuMesh = { (uint32_t)mesh.indices.size(), indexIndex, renderObject.transform->position(), materialIndex };
			glNamedBufferSubData(m_meshSSBO, meshIndex * sizeofll(GPUMesh), sizeofll(GPUMesh), &gpuMesh);
			const auto vertices = (int64_t)mesh.vertices.size();
			const auto indices = (int64_t)mesh.indices.size();
			glNamedBufferSubData(m_vertexSSBO, vertexIndex * sizeofll(VertexData), vertices * sizeofll(VertexData), mesh.vertices.data());
			glNamedBufferSubData(m_indexSSBO, indexIndex * sizeofll(uint32_t), indices * sizeofll(uint32_t), mesh.indices.data());
			meshIndex++;
			vertexIndex += vertices;
			indexIndex += indices;
		}
	}

	// Bind newly made buffers to the shader
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, m_meshSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, m_indexSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, m_vertexSSBO);
}


void ObjectRenderer::SetSphereData(const int idx, const vec3f position, const float radius, int materialIndex) const
{
    if (m_computeShader == nullptr) return;
    const std::string baseName = "Spheres[" + std::to_string(idx) + "]";
    m_computeShader->SetVec3(baseName + ".center", position);
    m_computeShader->SetFloat(baseName + ".radius", radius);

	const int numMaterials = MaterialRegister::Instance().GetNumMaterials();
	materialIndex = (materialIndex >= 0 && materialIndex < numMaterials) ? materialIndex + 1 : 0;

	m_computeShader->SetInt(baseName + ".materialIndex", materialIndex);
}


struct GPUMaterial
{
	vec3f diffuseColor;
	float specularity;
	vec3f specularColor;
	float shininess;
	vec3f emissionColor;
	float emissionStrength;
	int type;
	float glossiness;
	float refractivity;
	float refractionCoefficient;
	float PBR_Roughness;
	float PBR_Metallic;
	float PBR_Reflectance;
	float alphaX;
	float alphaY;
	float _padding[3];
};

void ObjectRenderer::UpdateMaterialSSBO()
{
    if (m_computeShader == nullptr) return;
	// Delete old buffer
	glDeleteBuffers(1, &m_materialSSBO);
	// Create a new one
	glGenBuffers(1, &m_materialSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_materialSSBO);

	// Determine the size
	glNamedBufferStorage(
		m_materialSSBO,
		(MaterialRegister::Instance().GetNumMaterials() + 1) * sizeofll(GPUMaterial),
		nullptr,
		GL_DYNAMIC_STORAGE_BIT
	);

	auto FillMaterial = [this](const Material* mat, const int idx) -> void
	{
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
		const GPUMaterial gpuMaterial = {
			.diffuseColor = mat->m_diffuseColor,
			.specularity = mat->m_specularity,
			.specularColor = mat->m_specularColor,
			.shininess = mat->m_shininess,
			.emissionColor = mat->m_emissionColor,
			.emissionStrength = mat->m_emissionStrength,
			.type= type,
			.glossiness = mat->m_glossiness,
			.refractivity = mat->m_refractivity,
			.refractionCoefficient = mat->m_refractionCoefficient,
			.PBR_Roughness = mat->m_PBR_Roughness,
			.PBR_Metallic = mat->m_PBR_Metallic,
			.PBR_Reflectance = mat->m_PBR_Reflectance,
			.alphaX = mat->m_microfacetModel.alphaX,
			.alphaY = mat->m_microfacetModel.alphaY,
			._padding= {0.f}
		};
		glNamedBufferSubData(m_materialSSBO, idx * sizeofll(GPUMaterial), sizeofll(GPUMaterial), &gpuMaterial);
	};

	// Add the default material to the buffer
	const Material* defaultMaterial = MaterialRegister::Instance().GetMaterial(-1);
	FillMaterial(defaultMaterial, 0);

	// Add the materials to the new buffer
	const auto& materials = MaterialRegister::Instance().GetMaterials();
    for (int i = 0; i < materials.size(); ++i)
    {
    	const Material* mat = materials[i];
    	FillMaterial(mat, i + 1);
    }

	// Bind the buffer
	m_computeShader->Use();
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, m_materialSSBO);
}

}

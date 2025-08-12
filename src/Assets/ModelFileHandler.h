#pragma once

namespace Engine
{

struct alignas(16) VertexData
{
	vec3f position;
	vec3u normalTangent;
	vec2f texCoords;
};

/** This struct is for internal use ONLY. Do NOT use it yourself */
struct GPUMesh
{
	uint32_t indexCount = 0;
	uint32_t firstIndex = 0;
	vec3f position;
	float _padding = 0.f;
};

struct MeshData
{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;

	GPUMesh gpuMesh;

	int materialIndex;
};

struct ModelData
{
	std::vector<MeshData> meshes;
};

/**
 * An editor only static class that handles 3D model files, stores the data in a LUT
 * and keeps track of where they are and if they are loaded into the engine
 */
class ModelFileHandler
{

public:

	static ModelData* LoadModel(const std::filesystem::path& modelFile);

private:

	static std::unordered_map<std::wstring, ModelData> m_loadedModels;

};

}

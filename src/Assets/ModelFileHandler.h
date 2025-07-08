#pragma once

namespace Engine
{

struct alignas(16) VertexData
{
	vec3f position;
	vec3f normal;
	vec3f tangent;
	vec2f texCoords;
};

struct MeshData
{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;

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

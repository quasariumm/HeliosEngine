#pragma once

#include "tiny_bvh.h"

namespace Engine
{

struct alignas(16) VertexData
{
	vec3f position;
	float texCoordX;
	vec3u normalTangent;
	float texCoordY;
};

struct MeshData
{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	std::vector<tinybvh::BVH_GPU::BVHNode> bvhNodes;
};

struct ModelData
{
	std::vector<MeshData> meshes;
};

struct ModelInstance
{
	ModelData* modelData;
	std::vector<int> materialIndices;
};

/**
 * An editor only static class that handles 3D model files, stores the data in a LUT
 * and keeps track of where they are and if they are loaded into the engine
 */
class ModelFileHandler
{

public:

	static ModelInstance LoadModel(const std::filesystem::path& modelFile);

private:

	static std::unordered_map<std::wstring, ModelData> m_loadedModels;

};

}

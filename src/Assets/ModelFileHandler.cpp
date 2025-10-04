#include "ModelFileHandler.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debugger/Debugger.h"

#define TINYBVH_IMPLEMENTATION
#include "tiny_bvh.h"

namespace Engine
{

static void InitMesh(MeshData& mesh, const std::wstring& directory, const aiMesh* aiMesh, const aiScene* aiScene)
{
	_CRT_UNUSED(directory);
	_CRT_UNUSED(aiScene);
	/*
	 * Vertices
	 */

	mesh.vertices = ALIGNED_NEW(64) VertexData[aiMesh->mNumVertices];
	mesh.numVertices = aiMesh->mNumVertices;
	auto* bvhvertices = (tinybvh::bvhvec4*)tinybvh::malloc64(aiMesh->mNumVertices * sizeof(tinybvh::bvhvec4));

	for (unsigned i = 0; i < aiMesh->mNumVertices; i++)
	{
		VertexData& vertex = mesh.vertices[i];
		vertex.position.x = aiMesh->mVertices[i].x;
		vertex.position.y = aiMesh->mVertices[i].y;
		vertex.position.z = aiMesh->mVertices[i].z;

		bvhvertices[i] = tinybvh::bvhvec4(vertex.position.x, vertex.position.y, vertex.position.z, 1);

		vertex.normalTangent.x = PackHalf2x16(aiMesh->mNormals[i].x, aiMesh->mTangents[i].x);
		vertex.normalTangent.y = PackHalf2x16(aiMesh->mNormals[i].y, aiMesh->mTangents[i].y);
		vertex.normalTangent.z = PackHalf2x16(aiMesh->mNormals[i].z, aiMesh->mTangents[i].z);

		if (aiMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			vertex.texCoordX = aiMesh->mTextureCoords[0][i].x;
			vertex.texCoordY = aiMesh->mTextureCoords[0][i].y;
		}
	}

	/*
	 * Indices
	 */

	size_t numIndices = 0;
	for (unsigned i = 0; i < aiMesh->mNumFaces; i++)
		numIndices += aiMesh->mFaces[i].mNumIndices;

	mesh.indices = ALIGNED_NEW(64) uint32_t[numIndices];
	mesh.numIndices = numIndices;

	uint32_t indexIndex = 0;
	for (unsigned i = 0; i < aiMesh->mNumFaces; i++)
	{
		const aiFace face = aiMesh->mFaces[i];
		memcpy(mesh.indices + indexIndex, face.mIndices, face.mNumIndices * sizeof(uint32_t));
		indexIndex += face.mNumIndices;
	}

	tinybvh::BVH_GPU bvh;
	bvh.Build(
		tinybvh::bvhvec4slice{ bvhvertices, aiMesh->mNumVertices, sizeof( tinybvh::bvhvec4 ) },
		mesh.indices,
		mesh.numIndices / 3
	);
	mesh.bvhNodes = ALIGNED_NEW(64) tinybvh::BVH_GPU::BVHNode[bvh.usedNodes];
	mesh.numBVHNodes = bvh.usedNodes;
	memcpy(mesh.bvhNodes, bvh.bvhNode, bvh.usedNodes * sizeof(tinybvh::BVH_GPU::BVHNode));
	// Use the indices that the bvh gave
	auto* newIndices = ALIGNED_NEW(64) uint32_t[3 * bvh.triCount];
	for (int i = 0; i < bvh.triCount; ++i)
	{
		uint32_t primIdx = 3 * bvh.bvh.primIdx[i];
		newIndices[3 * i + 0] = mesh.indices[primIdx + 0];
		newIndices[3 * i + 1] = mesh.indices[primIdx + 1];
		newIndices[3 * i + 2] = mesh.indices[primIdx + 2];
	}
	ALIGNED_LIST_DELETE(mesh.indices, 64);
	mesh.indices = newIndices;
	mesh.numIndices = 3 * bvh.triCount;

	tinybvh::free64(bvhvertices);

	/*
	 * Textures TODO
	 */
	// {
	// 	aiMaterial* material = aiScene->mMaterials[aiMesh->mMaterialIndex];
	//
	// 	LoadMaterialTextures(directory, mesh.textures, material, aiTextureType_DIFFUSE);
	//
	// 	LoadMaterialTextures(directory, mesh.textures, material, aiTextureType_NORMALS);
	// 	// Normals might be seen as height maps: https://github.com/assimp/assimp/issues/430
	// 	LoadMaterialTextures(directory, mesh.textures, material, aiTextureType_HEIGHT);
	//
	// 	LoadMaterialTextures(directory, mesh.textures, material, aiTextureType_SHININESS);
	// 	LoadMaterialTextures(directory, mesh.textures, material, aiTextureType_MAYA_SPECULAR_ROUGHNESS);
	// 	// LoadMaterialTextures(modelMesh.textures, material, aiTextureType_HEIGHT, "texture_height");
	// }
}

static void ProcessNode(const aiNode* node, const aiScene* scene, std::vector<MeshData>& meshes, const std::wstring& directory)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		const aiMesh* aiMesh = scene->mMeshes[node->mMeshes[i]];
		MeshData& mesh = meshes.emplace_back();
		InitMesh(mesh, directory, aiMesh, scene);
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, meshes, directory);
	}
}

std::unordered_map<std::wstring, ModelData> ModelFileHandler::m_loadedModels = {};


MeshData::~MeshData()
{
	ALIGNED_LIST_DELETE(vertices, 64);
	ALIGNED_LIST_DELETE(indices, 64);
	ALIGNED_LIST_DELETE(bvhNodes, 64);
}


ModelInstance ModelFileHandler::LoadModel( const std::filesystem::path& modelFile )
{
	if (m_loadedModels.contains(modelFile.wstring()))
	{
		ModelData* modelData = &m_loadedModels[modelFile.wstring()];
		return {
			.modelData=			modelData,
			.materialIndices=	std::vector<int>(modelData->meshes.size(), -1)
		};
	}

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(modelFile.string().c_str(),
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene
		|| scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode)
	{
		DebugLog(LogSeverity::SEVERE, std::format(L"Assimp error {0}.", STR_TO_WSTR(importer.GetErrorString()).c_str()));
		return {nullptr, std::vector<int>() };
	}

	m_loadedModels[modelFile.wstring()] = {};

	ProcessNode(scene->mRootNode, scene, m_loadedModels[modelFile.wstring()].meshes, modelFile.parent_path());

	ModelData* modelData = &m_loadedModels[modelFile.wstring()];
	return {
		.modelData=			modelData,
		.materialIndices=	std::vector<int>(modelData->meshes.size(), -1)
	};
}

}

#include "ModelFileHandler.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Debugger/Debugger.h"


namespace Engine
{

static void InitMesh(MeshData& mesh, const std::wstring& directory, const aiMesh* aiMesh, const aiScene* aiScene)
{
	_CRT_UNUSED(directory);
	_CRT_UNUSED(aiScene);
	/*
	 * Vertices
	 */

	for (unsigned i = 0; i < aiMesh->mNumVertices; i++)
	{
		VertexData& vertex = mesh.vertices.emplace_back();
		vertex.position.x = aiMesh->mVertices[i].x;
		vertex.position.y = aiMesh->mVertices[i].y;
		vertex.position.z = aiMesh->mVertices[i].z;

		vertex.normalTangent.x = PackHalf2x16(aiMesh->mNormals[i].x, aiMesh->mTangents[i].x);
		vertex.normalTangent.y = PackHalf2x16(aiMesh->mNormals[i].y, aiMesh->mTangents[i].y);
		vertex.normalTangent.z = PackHalf2x16(aiMesh->mNormals[i].z, aiMesh->mTangents[i].z);

		if (aiMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			vertex.texCoords.x = aiMesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = aiMesh->mTextureCoords[0][i].y;
		}
	}

	/*
	 * Indices
	 */

	for (unsigned i = 0; i < aiMesh->mNumFaces; i++)
	{
		const aiFace face = aiMesh->mFaces[i];
		for (unsigned idx = 0; idx < face.mNumIndices; idx += 3)
		{
			mesh.indices.emplace_back(face.mIndices[idx + 0]);
			mesh.indices.emplace_back(face.mIndices[idx + 1]);
			mesh.indices.emplace_back(face.mIndices[idx + 2]);
		}
	}

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

ModelData* ModelFileHandler::LoadModel( const std::filesystem::path& modelFile )
{
	if (m_loadedModels.contains(modelFile.wstring()))
		return &m_loadedModels[modelFile.wstring()];

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(modelFile.string().c_str(),
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene
		|| scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
		|| !scene->mRootNode)
	{
		DebugLog(LogSeverity::SEVERE, std::format(L"Assimp error {0}.", STR_TO_WSTR(importer.GetErrorString()).c_str()));
		return nullptr;
	}

	m_loadedModels[modelFile.wstring()] = {};

	ProcessNode(scene->mRootNode, scene, m_loadedModels[modelFile.wstring()].meshes, modelFile.parent_path());

	return &m_loadedModels[modelFile.wstring()];
}

}

//
// Created by Tygov on 05/11/2025.
//

#include "../include/asset_prep.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
#include <unordered_set>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "geometric.hpp"

namespace fs = std::filesystem;

File::File(const std::string& filePath) : path(filePath)
{
	fs::path p(filePath);
        
	name = p.stem().string();  // filename without extension
	extension = p.extension().string();
	if (!extension.empty() && extension[0] == '.') {
		extension = extension.substr(1);
	}
        
	std::transform(extension.begin(), extension.end(), 
				  extension.begin(), ::tolower);
        
	type = ClassifyExtension(extension);
	uuid = GenerateUUID();
}

std::string File::GenerateUUID()
{
	static std::random_device rd;
	static std::mt19937_64 gen(rd());
	static std::uniform_int_distribution<uint64_t> dis;
        
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	ss << std::setw(16) << dis(gen);
	ss << std::setw(16) << dis(gen);
	return ss.str();
}

FileType File::ClassifyExtension(const std::string& ext)
{
	static const std::unordered_set<std::string> textureExts = {
		"png", "jpg", "jpeg", "bmp", "tga", "dds", "hdr"
	};
	static const std::unordered_set<std::string> modelExts = {
		"obj", "fbx", "gltf", "glb", "dae"
	};
        
	if (textureExts.count(ext)) return TEXTURE;
	if (modelExts.count(ext)) return MODEL;
	return INVALID;
}

uint16_t Vertex::PackNormalToUint16(const glm::vec3& normal)
{
	// Normalize if not already
	glm::vec3 n = glm::normalize(normal);
    
	// Octahedral wrap
	glm::vec2 encoded = glm::vec2(n.x, n.y) / (abs(n.x) + abs(n.y) + abs(n.z));
	if (n.z < 0.0f) {
		glm::vec2 wrapped = glm::vec2(1.0f - abs(encoded.y), 1.0f - abs(encoded.x));
		encoded = glm::mix(wrapped, -wrapped, glm::vec2(encoded.x >= 0.0f ? 1.0f : 0.0f, encoded.y >= 0.0f ? 1.0f : 0.0f));
	}
    
	// Map from [-1, 1] to [0, 65535]
	auto x = static_cast<uint16_t>((encoded.x * 0.5f + 0.5f) * 65535.0f);
	auto y = static_cast<uint16_t>((encoded.y * 0.5f + 0.5f) * 65535.0f);
    
	return (x << 16) | y;  // Pack into single uint32
}

glm::uvec3 Vertex::PackNormalAndTangent(const glm::vec3& normal, const glm::vec3& tangent)
{
	glm::vec3 n = glm::normalize(normal);
	glm::vec3 t = glm::normalize(tangent);
    
	glm::uvec3 packed;
    
	// For each axis, pack normal in lower 16 bits, tangent in upper 16 bits
	for (int i = 0; i < 3; i++) {
		// Map from [-1, 1] to [0, 65535]
		auto normalPacked = static_cast<uint16_t>((n[i] * 0.5f + 0.5f) * 65535.0f);
		auto tangentPacked = static_cast<uint16_t>((t[i] * 0.5f + 0.5f) * 65535.0f);
        
		// Combine: lower 16 bits = normal, upper 16 bits = tangent
		packed[i] = (static_cast<uint32_t>(tangentPacked) << 16) | normalPacked;
	}
    
	return packed;
}

void AssetPrep::Run(const std::string& assetsPath)
{
	std::cout << "Started Asset Preparation" << std::endl;

	// Check if directory exists
	if (!fs::exists(assetsPath))
	{
		std::cerr << "Assets directory not found: " << assetsPath << std::endl;
		std::cerr << "Current working directory: " << fs::current_path() << std::endl;
		return;
	}
	
	for (const auto& entry : fs::recursive_directory_iterator(assetsPath))
	{
		File file(entry.path().string());
        
		switch (file.type)
		{
			case INVALID:
				std::cout << "Type of file: " << file.path << " is not supported" << std::endl;
				break;
                
			case TEXTURE:
			{
				std::string outputPath;
				std::ofstream stream = StartIntermediateFile(file, "TASN", outputPath);
				
				if (stream.is_open()) {
					GenerateMipmaps(file, stream);
					CompressTexture(file, stream);
					
					stream.close();
					std::cout << "Finalized: " << outputPath << std::endl;
				}
				
				CreateMetaFile(file);
				break;
			}
                
			case MODEL:
			{
				std::string outputPath;
				std::ofstream stream = StartIntermediateFile(file, "MASN", outputPath);
				
				if (stream.is_open())
				{
					AlignData(file, stream);
					GenerateLODs(file, stream);
					OptimizeVertices(file, stream);
					CompressVertexData(file, stream);
					CompressIndexData(file, stream);
					
					stream.close();
					std::cout << "Finalized: " << outputPath << std::endl;
				}
				
				CreateMetaFile(file);
				break;
			}
		}
	}
	std::cout << "Ended Asset Preparation" << std::endl;
}

std::ofstream AssetPrep::StartIntermediateFile(const File& file, const char* magic, std::string& outPath)
{
	fs::create_directories(pathToAssets + "intermediate");
    
	std::string extension = file.type == TEXTURE ? ".tex" : ".mesh";
	outPath               = pathToAssets + "intermediate/" + file.uuid + extension;
    
	std::ofstream stream(outPath, std::ios::binary);
    
	if (!stream.is_open()) {
		std::cerr << "Failed to create: " << outPath << std::endl;
		return stream;
	}
	
	WriteHeader(stream, magic, file.uuid);
	std::cout << "Started intermediate file: " << outPath << std::endl;
	
	return stream;
}

void AssetPrep::WriteHeader(std::ofstream& stream, const char* magic, const std::string& uuid)
{
	IntermediateHeader header;
	memcpy(header.magic, magic, 4);
	header.version = 1;
	memcpy(header.uuid, uuid.c_str(), std::min(uuid.size(), size_t(32)));
	header.dataSize = 0; // Will update later
    
	stream.write(reinterpret_cast<const char*>(&header), sizeof(header));
}

// Texture processing functions
void AssetPrep::GenerateMipmaps(const File& file, std::ofstream& stream)
{
	std::cout << "  - Generating mipmaps for " << file.name << std::endl;
	// TODO: Implement mipmap generation
}

void AssetPrep::CompressTexture(const File& file, std::ofstream& stream)
{
	std::cout << "  - Compressing texture " << file.name << std::endl;
	// TODO: Implement texture compression (BC4/BC5/BC7)
}

void AssetPrep::AlignData(const File& file, std::ofstream& stream)
{
	std::cout << "  - Alligning Data for " << file.name << std::endl;

// Load mesh using Assimp
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file.path, 
        aiProcess_Triangulate |           // Convert to triangles
        aiProcess_GenNormals |            // Generate normals if missing
        aiProcess_CalcTangentSpace |      // Generate tangents
        aiProcess_JoinIdenticalVertices | // Optimize
        aiProcess_ImproveCacheLocality);  // Optimize for GPU cache
    
    if (!scene || !scene->mMeshes || scene->mNumMeshes == 0) {
        std::cerr << "    Failed to load mesh: " << file.path << std::endl;
        return;
    }
    
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    
    // Process all meshes in the file
    for (uint32_t m = 0; m < scene->mNumMeshes; m++) {
        aiMesh* mesh = scene->mMeshes[m];
        uint32_t vertexOffset = vertices.size();
        
        // Extract vertices
        for (uint32_t v = 0; v < mesh->mNumVertices; v++) {
            Vertex vertex{};
            
            // Position
            vertex.position = glm::vec3(
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            );
            
            // Normal and Tangent (packed)
            glm::vec3 normal = glm::vec3(0, 1, 0);  // Default up
            glm::vec3 tangent = glm::vec3(1, 0, 0); // Default right
            
            if (mesh->HasNormals()) {
                normal = glm::vec3(
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                );
            }
            
            if (mesh->HasTangentsAndBitangents()) {
                tangent = glm::vec3(
                    mesh->mTangents[v].x,
                    mesh->mTangents[v].y,
                    mesh->mTangents[v].z
                );
            }
            
            vertex.normalTangent = vertex.PackNormalAndTangent(normal, tangent);
            
            // Texture coordinates
            if (mesh->HasTextureCoords(0)) {
                vertex.texCoordX = mesh->mTextureCoords[0][v].x;
                vertex.texCoordY = mesh->mTextureCoords[0][v].y;
            } else {
                vertex.texCoordX = 0.0f;
                vertex.texCoordY = 0.0f;
            }
            
            vertices.push_back(vertex);
        }
        
        // Extract indices
        for (uint32_t f = 0; f < mesh->mNumFaces; f++) {
            aiFace& face = mesh->mFaces[f];
            for (uint32_t i = 0; i < face.mNumIndices; i++) {
                indices.push_back(vertexOffset + face.mIndices[i]);
            }
        }
    }
    
    // Write vertex count and data
    uint32_t vertexCount = vertices.size();
    stream.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint32_t));
    stream.write(reinterpret_cast<const char*>(vertices.data()), 
                 vertexCount * sizeof(Vertex));
    
    // Write index count and data
    uint32_t indexCount = indices.size();
    stream.write(reinterpret_cast<const char*>(&indexCount), sizeof(uint32_t));
    stream.write(reinterpret_cast<const char*>(indices.data()), 
                 indexCount * sizeof(uint32_t));
    
    std::cout << "    Vertices: " << vertexCount << ", Indices: " << indexCount << std::endl;
	
}

// Model processing functions
void AssetPrep::GenerateLODs(const File& file, std::ofstream& stream)
{
	std::cout << "  - Generating LODs for " << file.name << std::endl;
	// TODO: Implement LOD generation
}

void AssetPrep::OptimizeVertices(const File& file, std::ofstream& stream)
{
	std::cout << "  - Optimizing vertices for " << file.name << std::endl;
	// TODO: Implement vertex optimization
}

void AssetPrep::CompressVertexData(const File& file, std::ofstream& stream)
{
	std::cout << "  - Compresssing vertices for " << file.name << std::endl;
	// TODO: Implement vertex optimization
}

void AssetPrep::CompressIndexData(const File& file, std::ofstream& stream)
{
	std::cout << "  - Compressing indices for " << file.name << std::endl;
	// TODO: Implement vertex optimization
}

void AssetPrep::CreateMetaFile(const File& file, const std::string& outputDir)
{
    
	std::string metaPath = outputDir + "/" + file.uuid + ".meta";
	std::ofstream metaFile(metaPath);
    
	if (metaFile.is_open()) {
		metaFile << "uuid: " << file.uuid << "\n";
		metaFile << "original_name: " << file.name << "." << file.extension << "\n";
		metaFile << "original_path: " << file.path << "\n";
		metaFile << "type: " << file.type << "\n";
		metaFile.close();
        
		std::cout << "Created meta file: " << metaPath << std::endl;
	}
}


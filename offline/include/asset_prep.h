//
// Created by Tygov on 05/11/2025.
//

#ifndef ASSET_PREP_H
#define ASSET_PREP_H
#include <filesystem>
#include <string>
#include <fstream>

#include <glm.hpp>

/*
 *	Go over all files in assets folder
 *  	Gen UUID
 *  	Choose category based on extension
 *  		Texture:
 *  			Load in
 *  			Gen mipmaps (FidelityFX-SPD)
 *  			Compress mipmaps
 *  				Single component → BC4
 *  				2 components → BC5
 *  				3 & 4 components → BC7
 *  				(+ Normal maps → BC5 with specific encoding)
 *  			Store metadata (format, mip count, dimensions)
 *  		
 *  		Model:
 *  			Load in
 *  			LOD gen (specify algorithm: quadric error, etc.)
 *  	
 *  			For each LOD:
 *  				Vertex optimization
 *  					- Cache optimization
 *  					- Fetch optimization
 *  					- Overdraw reduction
 *  				Vertex buffer compression (optional)
 *  				Index buffer compression
 *  			Store metadata (LOD distances, vertex counts)
 *
 */

const std::string pathToAssets = "../../../assets/";

enum FileType
{
	INVALID = 0,
	TEXTURE,
	MODEL
};

struct File {
	std::string path;
	std::string name;
	std::string extension;
	FileType type;
	std::string uuid;
    
	File(const std::string& filePath);
    
private:
	static std::string GenerateUUID();
	static FileType ClassifyExtension(const std::string& ext);
};

// Generic intermediate file header
struct IntermediateHeader {
	char magic[4];
	uint32_t version;
	char uuid[32];
	uint32_t dataSize;
};

struct Vertex
{
	glm::vec3 position;
	float texCoordX;
	glm::uvec3 normalTangent;  // Packed normal and tangent
	float texCoordY;
	
	// Helper: Pack a normalized vec3 into 16 bits using octahedral encoding
	uint16_t PackNormalToUint16(const glm::vec3& normal);
	// Pack normal (lower 16 bits) and tangent (upper 16 bits) into each axis
	glm::uvec3 PackNormalAndTangent(const glm::vec3& normal, const glm::vec3& tangent);
};


class AssetPrep
{
public:
	void Run(const std::string& assetsPath = "../../../assets");
	
private:
	// File creation helpers
	static std::ofstream StartIntermediateFile(const File& file, const char* magic, std::string& outPath);

	static void WriteHeader(std::ofstream& stream, const char* magic, const std::string& uuid);
	
	// Texture functions
	void GenerateMipmaps(const File& file, std::ofstream& stream);
	void CompressTexture(const File& file, std::ofstream& stream);
	
	// Model functions
	void AlignData(const File& file, std::ofstream& stream);
	void GenerateLODs(const File& file, std::ofstream& stream);
	void OptimizeVertices(const File& file, std::ofstream& stream);
	void CompressVertexData(const File& file, std::ofstream& stream);
	void CompressIndexData(const File& file, std::ofstream& stream);
	
	
	// Metadata
	static void CreateMetaFile(const File& file, const std::string& outputDir = "intermediate");
};

#endif //ASSET_PREP_H
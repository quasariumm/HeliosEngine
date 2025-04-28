#pragma once

#include "GL46_ShaderBase.h"


namespace Engine {

class GL46_ComputeShader final : public GL46_ShaderBase
{

public:

	// Note to the person reading this class's documentation:
	// On non-NVIDIA cards, the extension GL_ARB_shading_language_include is not supported.
	// Hence, we have to fiddle with the #line directives. This means that debugging shading compiler errors
	// can become quite cumbersome. We have tried out best to make it more user-friendly, but to be honest,
	// it might be the graphics card vendors at fault here for not supporting the amazing extension.
	// So when you get compiler errors on non-NVIDIA cards, you simply:
	// - Look in the loadedShaders set's values (can be found in src/Core/Common.h).
	// - Look at the name at the index it gives you + 1 (because 0 is always the shader you want to compile)

	GL46_ComputeShader() = default;

	~GL46_ComputeShader();

	/**
	 * @brief Loads and compiles the given compute shader
	 * @attention This function deletes the old shader if it was already initialised
	 * @note Due to GL limits, you might experience weird behaviour if the shader file includes Unicode characters
	 * @param filename The relative path to the shader file (from the executable working directory)
	 * @param spirV Whether the shader should be loaded from a GLSL file or a SPIR-V binary file
	 */
	void LoadFromFile( const std::wstring& filename, bool spirV = false );

	/**
	 * @brief Dispatches the loaded compute shader
	 * @param threads The number of threads per dimension (X, Y, Z)
	 * @throws runtime_error When no shader is initialised
	 */
	void Dispatch(const vec3u& threads) const;

private:

	static void ManageIncludes(
		std::wstringstream& contents, std::wostringstream& outStream,
		bool supportsGLSLIncludes, uint32_t currentFileIndex = 0
	);

	uint32_t m_shaderID = 0;

	bool m_initialised = false;

};

} // Engine

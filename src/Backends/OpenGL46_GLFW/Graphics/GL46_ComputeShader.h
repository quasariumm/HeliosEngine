#pragma once

#include "GL46_ShaderBase.h"


namespace Engine {

class GL46_ComputeShader final : public GL46_ShaderBase
{

public:

	GL46_ComputeShader() = default;

	~GL46_ComputeShader();

	/**
	 * @brief Loads and compiles the given compute shader
	 * @attention This function deletes the old shader if it was already initialised
	 * @note Due to GL limits, you might experience weird behaviour if the shader file includes Unicode characters
	 * @param filename The relative path to the shader file (from the executable working directory)
	 */
	void LoadFromFile(const std::wstring& filename);

	/**
	 * @brief Dispatches the loaded compute shader
	 * @param threads The number of threads per dimension (X, Y, Z)
	 * @throws runtime_error When no shader is initialised
	 */
	void Dispatch(const vec3u& threads) const;

private:

	uint32_t m_shaderID = 0;

	bool m_initialised = false;

};

} // Engine

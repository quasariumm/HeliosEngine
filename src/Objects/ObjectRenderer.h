#pragma once
#include "Backends/OpenGL46_GLFW/Graphics/GL46_ComputeShader.h"
#include "Scene/SceneObjects.h"

namespace Engine
{
struct ModelData;


enum class PrimitiveType
{
    SPHERE,
    CUBE,
    MODEL
};

struct RenderObject
{
    PrimitiveType primitiveType;
    Transform* transform;
    // Model specific
	ModelData** modelDataLoc = nullptr;
    // Sphere specific
    float* radius;
	int* materialIdx;
};

class ObjectRenderer
{
public:

	ObjectRenderer()
		: m_computeShader(nullptr), m_meshSSBO(0), m_vertexSSBO(0), m_indexSSBO(0), m_materialSSBO(0) {};

	virtual ~ObjectRenderer();

	static ObjectRenderer& Instance()
	{
		static ObjectRenderer instance;
		return instance;
	}

    /**
     * @brief Set the shader the objects will be sent to
     */
    void SetShader(GL46_ComputeShader* shader);

    /**
     * @brief Takes all the given objects and sends them to the GPU for rendering. Only run this when updating many objects!
     */
    void SendObjectData();

    /**
     * @brief Register an object to the renderer
     * @param transform A pointer to the transform of the object
     * @param modelDataLoc The adress of the model data for an instance of the Model component
     */
    void RegisterModelInstance(Transform* transform, ModelData** modelDataLoc);

	/**
	 * @brief Deregister an object to the renderer
	 * @param modelDataLoc The adress of the model data for the instance of a Model component
	 */
	void DeregisterModelInstance(ModelData** modelDataLoc);

    /**
     * @brief Register an object to the renderer
     * @param transform A pointer to the transform of the object
     * @param radius A pointer to the radius of the sphere
     * @param materialIdx The index in the material list
     */
    void RegisterSphere(Transform* transform, float* radius, int* materialIdx);

	/**
     * @brief Register an object to the renderer
     * @param radius A pointer to the radius of the sphere
     * @param materialIdx The index in the material list
     */
    void DeregisterSphere(float* radius, int* materialIdx);

	/**
	 * @brief Can be called to manually update the SSBOs
	 */
	void UpdateModelSSBOs();

	/**
	 * @brief Can be called to manually update the SSBO
	 */
	void UpdateMaterialSSBO();

private:

    void SetSphereData(int idx, vec3f position, float radius, int materialIndex) const;

    std::vector<RenderObject> m_renderObjects;
    GL46_ComputeShader* m_computeShader;

	uint32_t m_meshSSBO;
	uint32_t m_vertexSSBO;
	uint32_t m_indexSSBO;
	uint32_t m_materialSSBO;
};
}

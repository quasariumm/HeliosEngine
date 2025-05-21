#pragma once
#include "Backends/OpenGL46_GLFW/Graphics/GL46_ComputeShader.h"
#include "Scene/SceneObjects.h"

namespace Engine
{
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
    uint32_t modelUID;
    // Sphere specific
    float* radius;
	int* materialIdx;
};

class ObjectRenderer
{
public:
    /**
     * @brief Set the shader the objects will be sent to
     */
    static void SetShader(GL46_ComputeShader* shader) { m_computeShader = shader; }

    /**
     * @brief Takes all the given objects and sends them to the GPU for rendering. Only run this when updating many objects!
     */
    static void SendObjectData();

    /**
     * @brief Register an object to the renderer
     * @param transform A pointer to the transform of the object
     * @param modelUID The UID of the 3D model
     */
    static void RegisterModelInstance(Transform* transform, uint32_t modelUID);

    /**
     * @brief Register an object to the renderer
     * @param transform A pointer to the transform of the object
     * @param radius A pointer to the radius of the sphere
     * @param materialIdx The index in the material list
     */
    static void RegisterSphere(Transform* transform, float* radius, int* materialIdx);

private:
    static void SetSphereData(int idx, vec3 position, float radius);

    /**
     * Sets the uniform for an object's material data
     * @param base The base name for uniform setting
     * @param materialIdx The index of the material in the global material list
     * @throws std::range_error When the material index is less than zero or more than the array is long
     */
    static void SetMaterialData( const std::string& base, int materialIdx );

    static std::vector<RenderObject> m_renderObjects;
    static GL46_ComputeShader* m_computeShader;
};
}

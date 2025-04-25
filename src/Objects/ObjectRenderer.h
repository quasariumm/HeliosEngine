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
     */
    static void RegisterSphere(Transform* transform, float* radius);

private:
    static void SetSphereData(int idx, vec3 position, float radius);

    static void SetMaterialData(const std::string& base);

    static std::vector<RenderObject> m_renderObjects;
    static GL46_ComputeShader* m_computeShader;
};
}

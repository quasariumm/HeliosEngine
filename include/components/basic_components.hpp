#pragma once

#include <glm/glm.hpp>

#include "cereal/archives/json.hpp"
#include "core/ecs.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace glm {
template<class Archive>
void serialize(Archive& archive, glm::vec3& v) {
    archive(CEREAL_NVP(v.x), CEREAL_NVP(v.y), CEREAL_NVP(v.z));
}
    
template<class Archive>
void serialize(Archive& archive, glm::quat& q) {
    archive(CEREAL_NVP(q.w), CEREAL_NVP(q.x), CEREAL_NVP(q.y), CEREAL_NVP(q.z));
}
}

namespace Engine::Components
{

struct Transform : BaseComponent
{
    Transform() = default;

    // Setters
    // Position
    void SetPosition    (const glm::vec3& new_position) { MarkDirty(); position   = new_position; }
    void OffsetPosition (const glm::vec3& new_position) { MarkDirty(); position  += new_position; }

    // Rotation
    void SetRotation       (const glm::quat& new_rotation) { MarkDirty(); rotation   = new_rotation; }
    void SetEulerRotation  (const glm::vec3& new_rotation) { MarkDirty(); rotation   = glm::quat(glm::radians(new_rotation)); }
    
    void SetEulerRotationX (const float& degrees) { MarkDirty(); glm::vec3 temp = GetEulerRotation(); temp.x = glm::radians(degrees); rotation = glm::quat(temp); }
    void SetEulerRotationY (const float& degrees) { MarkDirty(); glm::vec3 temp = GetEulerRotation(); temp.y = glm::radians(degrees); rotation = glm::quat(temp); }
    void SetEulerRotationZ (const float& degrees) { MarkDirty(); glm::vec3 temp = GetEulerRotation(); temp.z = glm::radians(degrees); rotation = glm::quat(temp); }

    void OffsetEulerRotationX (const float& degrees) { MarkDirty(); glm::vec3 temp = GetEulerRotation(); temp.x += glm::radians(degrees); rotation = glm::quat(temp); }
    void OffsetEulerRotationY (const float& degrees) { MarkDirty(); glm::vec3 temp = GetEulerRotation(); temp.y += glm::radians(degrees); rotation = glm::quat(temp); }
    void OffsetEulerRotationZ (const float& degrees) { MarkDirty(); glm::vec3 temp = GetEulerRotation(); temp.z += glm::radians(degrees); rotation = glm::quat(temp); }
    
    void RotateX (const float& degrees) { RotateAxis({1,0,0}, degrees); }
    void RotateY (const float& degrees) { RotateAxis({0,1,0}, degrees); }
    void RotateZ (const float& degrees) { RotateAxis({0,0,1}, degrees); }
    
    void RotateAxis (const glm::vec3& axis, const float& degrees)
    {
        MarkDirty();
        glm::quat rot = glm::angleAxis(glm::radians(degrees), glm::normalize(axis));
        rotation = rot * rotation;
    }
    
    // Scale
    void SetScale (const glm::vec3& new_scale) { MarkDirty(); scale = new_scale; }

    // Getters
    [[nodiscard]] glm::vec3 GetPosition() const { return position; }
    [[nodiscard]] glm::quat GetRotation() const { return rotation; }
    [[nodiscard]] glm::vec3 GetEulerRotation() const { return glm::eulerAngles(rotation); }
    [[nodiscard]] glm::vec3 GetScale   () const { return scale;    }
    
    [[nodiscard]] glm::vec3 GetForward () const { return glm::normalize(GetMatrix()[1]); }
    [[nodiscard]] glm::vec3 GetRight   () const { return glm::normalize(glm::cross(GetForward(), {0,0,1})); }
    [[nodiscard]] glm::vec3 GetUp      () const { return glm::normalize(glm::cross(GetRight(), GetForward())); }
    
    [[nodiscard]] glm::mat4 GetMatrix() const
    {
        if (dirty)
        {
            glm::mat4 model_translate = glm::translate(glm::mat4(1.0f), position);
            glm::mat4 model_rotate = glm::mat4_cast(rotation);
            glm::mat4 model_scale = glm::scale(glm::mat4(1.0f), scale);
            transform = model_translate * model_rotate * model_scale;
            
            dirty = false;
        }
        return transform;
    }

    // Assuming that when a reference is used, that it will be used to change the value
    
    glm::vec3* PositionRef() { MarkDirty(); return &position; }
    glm::quat* RotationRef() { MarkDirty(); return &rotation; }
    glm::vec3* ScaleRef()    { MarkDirty(); return &scale;    }

    template <class Archive>
    void save(Archive & ar) const
    {
        ar(CEREAL_NVP(position), CEREAL_NVP(rotation), CEREAL_NVP(scale));
    }
    
    template <class Archive>
    void load(Archive & ar)
    {
        MarkDirty();
        ar(CEREAL_NVP(position), CEREAL_NVP(rotation), CEREAL_NVP(scale));
    }

    template <auto Candidate>
    void SubscribeOnModify()
    {
        onModifiedUsed = true;
        onModified.connect<Candidate>();
    }

    template <auto Candidate, typename Type>
    void SubscribeOnModify(Type* instance)
    {
        onModifiedUsed = true;
        onModified.connect<Candidate>(instance);
    }

    void Inspector() override;
    
private:
    void MarkDirty() const
    {
        dirty = true;
    }

    entt::delegate<void()> onModified {};
    bool onModifiedUsed = false;
    
    glm::vec3 position = glm::vec3(0);
    glm::quat rotation = glm::vec3(0);
    glm::vec3 scale    = glm::vec3(1);
    
    mutable glm::mat4 transform = glm::identity<glm::mat4>();
    
    mutable bool dirty = true;
};

REGISTER_COMPONENT(Transform, ICON_AXIS_ARROW" Transform", INSPECTABLE);

struct SceneObjectInfo : BaseComponent
{
    SceneObjectInfo() = default;
    explicit SceneObjectInfo(const std::string& objectName) { name = objectName; }
    
    std::string name = "Object";

    template<typename Archive>
    void serialize(Archive &archive) { archive(CEREAL_NVP(name)); }
};

REGISTER_COMPONENT(SceneObjectInfo, "SceneObject", NONE);

// Add to object to hide from renderer
struct Hidden {};

struct ParentObject : BaseComponent
{
    void RemoveChild(entt::entity child)
    {
        children.erase(std::find(children.begin(), children.end(), child));
    }
    void AddChild(entt::entity child)
    {
        children.push_back(child);
    }
    
    std::vector<entt::entity> children;
};

struct ChildObject : BaseComponent
{
    entt::entity parent = entt::null;

    template<typename Archive>
    void serialize(Archive &archive) { archive(CEREAL_NVP(parent)); }
};

REGISTER_COMPONENT(ChildObject, "Child Object", NONE);

// TODO(Patrick): Make a renderable component pls :)
// struct RenderObject : BaseComponent
// {
//     RenderObject() = default;
//     RenderObject(const resources::ResourceHandle<resources::Model>& resource) : model(resource) {}
//
//     resources::ResourceHandle<resources::Model> model = {};
//
//     template<typename Archive>
//     std::string save_minimal(Archive const &) const
//     {
//         return model.rawData->filePath.generic_string();
//     }
//
//     template<typename Archive>
//     void load_minimal(Archive const &, std::string const & value)
//     {
//         model = resources::LoadResource<resources::Model>(value);
//     }
// };
//
// REGISTER_COMPONENT(RenderObject, "Render Object", NONE);


struct DebugCube : BaseComponent
{
    DebugCube() = default;
    DebugCube(glm::vec3 c) { color = c; }
    
    glm::vec3 color = glm::vec3(1);
        
    template<typename Archive>
    void serialize(Archive &archive) { archive(CEREAL_NVP(color)); }

    void Inspector() override;
};

REGISTER_COMPONENT(DebugCube, ICON_CUBE_OUTLINE" Debug Cube", INSPECTABLE | ADDABLE);

struct Camera : BaseComponent
{
    float fov = 65.0f;
    float near_plane = 0.01f;
    float far_plane = 1000.0f;

    template<typename Archive>
    void serialize(Archive &archive) { archive(CEREAL_NVP(fov), CEREAL_NVP(near_plane), CEREAL_NVP(far_plane)); }

    void Inspector() override;
};

REGISTER_COMPONENT(Camera, ICON_VIDEO" Camera", ADDABLE | INSPECTABLE);

// Add to object to mark for delete
struct DeleteMarker {};
    

}

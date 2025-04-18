#pragma once

#include "Components/Component.h"
#include "Debugger/Debugger.h"

class Model;

namespace Engine
{
    /// Basic transform with position, rotation and scale. Basic use of rotation is in Euler angles
    struct Transform
    {
        // Getter functions
        [[nodiscard]]
        mat4 matrix(bool inverse = false) const { return inverse ? it : t; }
        [[nodiscard]]
        vec3 position() const { return p; }
        [[nodiscard]]
        vec3* positionRef() { return &p; }
        [[nodiscard]]
        vec3 rotation() const { return r; }
        [[nodiscard]]
        vec3* rotationRef() { return &r; }
        [[nodiscard]]
        vec3 scale() const { return s; }
        [[nodiscard]]
        vec3* scaleRef() { return &s; }

        // Setter functions
        void matrix(const mat4& matrix) { t = matrix, UpdateTransform(); }
        void position(const vec3& position) { p = position, UpdateTransform(); }
        void rotation(const vec3& rotation) { r = rotation, UpdateTransform(); }
        void scale(const vec3& scale) { s = scale, UpdateTransform(); }

        void UpdateTransform()
        {
            // t = mat4::Translate(p) * mat4::Scale(s) * mat4::RotateX(DEG2RAD(r.x)) * mat4::RotateY(DEG2RAD(r.y)) *
            //     mat4::RotateZ(DEG2RAD(r.z));
            // it = t.Inverted();
        }

        /// Display options to change this transform
        void TransformControllerUI();

    private:
        vec3 p = {0,0,0};
        vec3 r = {0,0,0};
        vec3 s = {1,1,1};

        mat4 t = mat4();
        mat4 it = Invert(mat4());
    };

    static std::map<std::string, Model*> gLoadedModels = {};

    static Model* LoadModel(char* filePath);

    /// The base of any object. Defines the object type, transform, parent, and has a unique identifier (UID)
    class SceneObject
    {
    public:
        explicit SceneObject(const uint32_t UID) : m_UID(UID) { }
        ~SceneObject();

        [[nodiscard]]
        uint32_t GetUID() const { return m_UID; }
        Transform* GetTransform() { return &m_transform; }
        /// Completely overwrite the transform
        void SetTransform(const Transform& transform) { m_transform = transform; }

        [[nodiscard]]
        std::string GetName() const { return m_displayName; }
        void SetName(const std::string& name) { m_displayName = name; }

        [[nodiscard]]
        mat4 GlobalMatrix() const;

        /// Set the parent of the object. Also assigns it as a child of the parent
        void SetParent(SceneObject* object);

        /// Add an object as a child. Also marks this object as the parent
        void AddChild(SceneObject* object)
        {
            object->SetParent(this);
        }

        /// Remove an object as a child. Also removes parent from the child
        void RemoveChild(const SceneObject* object);

        template <typename T>
        T* FindComponent()
        {
            for (const std::unique_ptr<Component>& c : m_components)
                if (c->GetType() == typeid(T))
                    return static_cast<T*>(c);
            DebugLog(LogSeverity::WARNING, "Object did not have requested component");
            return nullptr;
        }

        template <typename T>
        T* AddComponent()
        {
            auto component = std::make_unique<T>();
            T* ptr = component.get(); // raw pointer for access
            m_components.push_back(std::move(component));
            return ptr;
        }

        Component* AddComponentByName(const std::string& name);

        void RemoveComponent() { }

        const std::vector<std::unique_ptr<Component>>& GetComponentList() {return m_components;}
        
        std::vector<SceneObject*>& GetChildren() { return m_childObjects; }

        [[nodiscard]]
        SceneObject* GetParent() const { return m_parentObject; }

    private:
        const uint32_t m_UID = 0;
        std::string m_displayName = "Object";
        Transform m_transform = Transform();
        SceneObject* m_parentObject = nullptr;
        std::vector<SceneObject*> m_childObjects = {};
        std::vector<std::unique_ptr<Component>> m_components = {};
    };
}

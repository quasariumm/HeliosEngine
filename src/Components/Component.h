#pragma once
#include <imgui.h>
#include <map>
#include <utility>

namespace Engine
{
    class SceneObject;

    /// To create a component, add this struct as a variable to your class. You can then assign that variable to an object.
    struct Component
    {
        Component(const std::type_info& objectType, void* rawComponent, std::string  name ): m_componentType(objectType), m_rawComponent(rawComponent), m_displayName(std::move(name))
        {
        }

        void AttachToObject(SceneObject* object) { m_attachedObject = object; }
        [[nodiscard]] const std::type_info& GetType() const { return m_componentType; }
        [[nodiscard]] void* GetRawComponent() const { return m_rawComponent; }
        [[nodiscard]] std::string GetName() const {return m_displayName; }
        [[nodiscard]] SceneObject* GetAttachedObject() const { return m_attachedObject; }
        
        void AssignProperty(const std::string& displayName, int& valueRef) { m_intProperties.insert({ displayName, valueRef }); }
        void AssignProperty(const std::string& displayName, float& valueRef) { m_floatProperties.insert({ displayName, valueRef }); }
        void AssignProperty(const std::string& displayName, vec2& valueRef) { m_vec2Properties.insert({ displayName, valueRef }); }
        void AssignProperty(const std::string& displayName, vec3& valueRef) { m_vec3Properties.insert({ displayName, valueRef }); }

        void DisplayProperties() const
        {
            for (std::pair<std::string, int&> p : m_intProperties)
                ImGui::InputInt(p.first.c_str(), &p.second);
            for (std::pair<std::string, float&> p : m_floatProperties)
                ImGui::InputFloat(p.first.c_str(), &p.second);
            for (std::pair<std::string, vec2&> p : m_vec2Properties)
                ImGui::InputFloat2(p.first.c_str(), p.second.cell);
            for (std::pair<std::string, vec3&> p : m_vec3Properties)
                ImGui::InputFloat3(p.first.c_str(), p.second.cell);
        }
        
    private:
        SceneObject* m_attachedObject = nullptr;
        const std::type_info& m_componentType;
        const std::string m_displayName;
        void* m_rawComponent;

        std::map<std::string, int&> m_intProperties;
        std::map<std::string, float&> m_floatProperties;
        std::map<std::string, vec2&> m_vec2Properties;
        std::map<std::string, vec3&> m_vec3Properties;
    };
}

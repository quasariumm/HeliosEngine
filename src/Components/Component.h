#pragma once

#include <utility>

#include "Core/FileTools.h"

namespace Engine
{
    class SceneObject;

    struct ComponentProperty
    {
        std::wstring name;
        std::wstring type;
        const std::type_info& rawType;
        void* value;
    };

    class Component
    {
    public:
        Component() = delete;

        template<typename T>
        explicit Component(T* _component, std::wstring displayName):
        m_componentType(typeid(T)), m_displayName(std::move(displayName)) { }

        void AttachToObject(SceneObject* object) { m_attachedObject = object; }
        [[nodiscard]] const std::type_info& GetType() const { return m_componentType; }
        [[nodiscard]] std::wstring GetName() const {return m_displayName; }
        [[nodiscard]] SceneObject* GetAttachedObject() const { return m_attachedObject; }

        template<typename T>
        void AssignProperty(const std::wstring& name, T* value)
        {
        	const std::string demangled = Demangle(typeid(T).name());
            m_properties.emplace_back(name, std::wstring(demangled.begin(), demangled.end()), typeid(T), value);
        }

        template<typename T>
        void SetPropertyValue(const std::wstring& propertyName, const T value)
        {
            for (const ComponentProperty& p : m_properties)
                if (p.name == propertyName)
                    *(T*)p.value = value;
        }

        void AssignProperty(const std::wstring& displayName, bool& valueRef) { m_boolProperties.insert({ displayName, valueRef }); }
        void AssignProperty(const std::wstring& displayName, int& valueRef) { m_intProperties.insert({ displayName, valueRef }); }
        void AssignProperty(const std::wstring& displayName, float& valueRef) { m_floatProperties.insert({ displayName, valueRef }); }
        void AssignProperty(const std::wstring& displayName, vec2& valueRef) { m_vec2Properties.insert({ displayName, valueRef }); }
        void AssignProperty(const std::wstring& displayName, vec3& valueRef) { m_vec3Properties.insert({ displayName, valueRef }); }
        void AssignProperty(const std::wstring& displayName, std::wstring& valueRef) { m_textProperties.insert({ displayName, valueRef }); }

        void DisplayProperties() const
        {
            for (const ComponentProperty& p : m_properties)
            {
                if (p.rawType == typeid(bool)) ImGui::Checkbox(WStringToUTF8(p.name).c_str(), (bool*)p.value);
                if (p.rawType == typeid(int)) ImGui::InputInt(WStringToUTF8(p.name).c_str(), (int*)p.value);
                if (p.rawType == typeid(float)) ImGui::InputFloat(WStringToUTF8(p.name).c_str(), (float*)p.value);
                if (p.rawType == typeid(vec2)) ImGui::InputFloat2(WStringToUTF8(p.name).c_str(), ((vec2*)p.value)->cell);
                if (p.rawType == typeid(vec3)) ImGui::InputFloat3(WStringToUTF8(p.name).c_str(), ((vec3*)p.value)->cell);
            }

            for (std::pair<std::wstring, bool&> p : m_boolProperties)
                ImGui::Checkbox(WStringToUTF8(p.first).c_str(), &p.second);
            for (std::pair<std::wstring, int&> p : m_intProperties)
                ImGui::InputInt(WStringToUTF8(p.first).c_str(), &p.second);
            for (std::pair<std::wstring, float&> p : m_floatProperties)
                ImGui::InputFloat(WStringToUTF8(p.first).c_str(), &p.second);
            for (std::pair<std::wstring, vec2&> p : m_vec2Properties)
                ImGui::InputFloat2(WStringToUTF8(p.first).c_str(), p.second.cell);
            for (std::pair<std::wstring, vec3&> p : m_vec3Properties)
                ImGui::InputFloat3(WStringToUTF8(p.first).c_str(), p.second.cell);
        }

        const std::vector<ComponentProperty>& GetProperties() const { return m_properties; }

        const std::unordered_map<std::wstring, bool&>& GetBoolProperties() const { return m_boolProperties; }
        const std::unordered_map<std::wstring, int&>& GetIntProperties() const { return m_intProperties; }
        const std::unordered_map<std::wstring, float&>& GetFloatProperties() const { return m_floatProperties; }
        const std::unordered_map<std::wstring, vec2&>& GetVec2Properties() const { return m_vec2Properties; }
        const std::unordered_map<std::wstring, vec3&>& GetVec3Properties() const { return m_vec3Properties; }
        const std::unordered_map<std::wstring, std::wstring&>& GetTextProperties() const { return m_textProperties; }

    protected:
        SceneObject* m_attachedObject = nullptr;
        const std::type_info& m_componentType;
        const std::wstring m_displayName;

        std::vector<ComponentProperty> m_properties;

        std::unordered_map<std::wstring, bool&> m_boolProperties;
        std::unordered_map<std::wstring, int&> m_intProperties;
        std::unordered_map<std::wstring, float&> m_floatProperties;
        std::unordered_map<std::wstring, vec2&> m_vec2Properties;
        std::unordered_map<std::wstring, vec3&> m_vec3Properties;
        std::unordered_map<std::wstring, std::wstring&> m_textProperties;
    };

    class ComponentRegister
    {
        using Creator = std::function<std::unique_ptr<Component>()>;
    public:
        static ComponentRegister& Instance() {
            static ComponentRegister instance;
            return instance;
        }

        void Register(const std::wstring& name, Creator creator) {
            m_registry[name] = std::move(creator);
        }

        const std::vector<std::wstring>& GetComponentNames()
        {
            if (m_registryNames.empty())
                CreateComponentNameList();
            return m_registryNames;
        }

        std::unique_ptr<Component> Create(const std::wstring& name) {
            auto it = m_registry.find(name);
            if (it != m_registry.end()) {
                return it->second();
            }
            return nullptr;
        }

        void CreateComponentNameList()
        {
            for (std::pair<std::wstring, Creator> p : m_registry)
                m_registryNames.push_back(p.first);
        }

    private:
        std::unordered_map<std::wstring, Creator> m_registry;
        std::vector<std::wstring> m_registryNames;

    };

#define REGISTER_COMPONENT(TYPE) \
static bool TYPE##_registered = [](){ \
std::string type = #TYPE; \
Engine::ComponentRegister::Instance().Register(std::wstring(type.begin(), type.end()), []() { return std::make_unique<TYPE>(); }); \
return true; \
}()

}

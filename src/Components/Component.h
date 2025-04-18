#pragma once

#include <utility>

namespace Engine
{
    class SceneObject;

    class Component
    {
    public:
        Component() = delete;

        template<typename T>
        explicit Component(T* _component, std::string displayName):
        m_componentType(typeid(T)), m_displayName(std::move(displayName)) { }

        void AttachToObject(SceneObject* object) { m_attachedObject = object; }
        [[nodiscard]] const std::type_info& GetType() const { return m_componentType; }
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

    protected:
        SceneObject* m_attachedObject = nullptr;
        const std::type_info& m_componentType;
        const std::string m_displayName;

        std::unordered_map<std::string, int&> m_intProperties;
        std::unordered_map<std::string, float&> m_floatProperties;
        std::unordered_map<std::string, vec2&> m_vec2Properties;
        std::unordered_map<std::string, vec3&> m_vec3Properties;
    };

    class ComponentRegister
    {
        using Creator = std::function<std::unique_ptr<Component>()>;
    public:
        static ComponentRegister& Instance() {
            static ComponentRegister instance;
            return instance;
        }

        void Register(const std::string& name, Creator creator) {
            m_registry[name] = std::move(creator);
        }

        const std::vector<std::string>& GetComponentNames()
        {
            if (m_registryNames.empty())
                CreateComponentNameList();
            return m_registryNames;
        }

        std::unique_ptr<Component> Create(const std::string& name) {
            auto it = m_registry.find(name);
            if (it != m_registry.end()) {
                return it->second();
            }
            return nullptr;
        }

        void CreateComponentNameList()
        {
            for (std::pair<std::string, Creator> p : m_registry)
                m_registryNames.push_back(p.first);
        }

    private:
        std::unordered_map<std::string, Creator> m_registry;
        std::vector<std::string> m_registryNames;

    };

#define REGISTER_COMPONENT(TYPE) \
static bool TYPE##_registered = [](){ \
Engine::ComponentRegister::Instance().Register(#TYPE, []() { return std::make_unique<TYPE>(); }); \
return true; \
}()

}
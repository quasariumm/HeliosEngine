#pragma once

#include <utility>

#include "Core/FileTools.h"

namespace Engine
{
    class SceneObject;

    struct alignas(32) ComponentProperty
    {
        std::wstring name;
        std::wstring type;
        const std::type_info& rawType;
        void* value;
    	void(* displayFunc)(void*) = nullptr;
    };

    class Component
    {
    public:
        virtual ~Component() = default;
        Component() = delete;

        template<typename T>
        explicit Component(T* _component, std::wstring displayName):
			m_componentType(typeid(T)), m_displayName(std::move(displayName)) { }

        void AttachToObject(SceneObject* object) { m_attachedObject = object; Init(); }

        [[nodiscard]] const std::type_info& GetType() const { return m_componentType; }
        [[nodiscard]] std::wstring GetName() const {return m_displayName; }
        [[nodiscard]] SceneObject* GetAttachedObject() const { return m_attachedObject; }


        /**
         * @brief Assigns a named property to the component
         * @tparam T The type of the property
         * @param name The name of the property
         * @param value A pointer to the value
         * @param displayFunc (Optional for defaulted types) A custom function to display this property
         * @attention For non-defaulted types, you MUST supply a custom display function
         */
        template<typename T>
        void AssignProperty(const std::wstring& name, T* value, void(* displayFunc)(void*) = nullptr)
        {
        	const std::string demangled = Demangle(typeid(T).name());
            m_properties.emplace_back(name, std::wstring(demangled.begin(), demangled.end()), typeid(T), value, displayFunc);
        }


        /**
         * @brief Sets the value of a property
         * @tparam T The property type
         * @param propertyName The name of the property
         * @param value The value you want it to have
         */
        template<typename T>
        void SetPropertyValue(const std::wstring& propertyName, const T value)
        {
            for (const ComponentProperty& p : m_properties)
                if (p.name == propertyName)
                    *(T*)p.value = value;
        }


        /**
         * @brief Displays all the properties of the component
         * @throws std::runtime_error When a non-defaulted type does not have a custom display function set
         */
        void DisplayProperties() const
        {
            for (const ComponentProperty& p : m_properties)
            {
            	if (p.displayFunc) p.displayFunc(p.value);
                else if (p.rawType == typeid(bool)) ImGui::Checkbox(WStringToUTF8(p.name).c_str(), (bool*)p.value);
                else if (p.rawType == typeid(int)) ImGui::InputInt(WStringToUTF8(p.name).c_str(), (int*)p.value);
                else if (p.rawType == typeid(float)) ImGui::InputFloat(WStringToUTF8(p.name).c_str(), (float*)p.value);
                else if (p.rawType == typeid(vec2)) ImGui::InputFloat2(WStringToUTF8(p.name).c_str(), ((vec2*)p.value)->cell);
                else if (p.rawType == typeid(vec3)) ImGui::InputFloat3(WStringToUTF8(p.name).c_str(), ((vec3*)p.value)->cell);
            	else throw std::runtime_error("The property has a non-defaulted type but does not have a custom display function");
            }
        }

        const std::vector<ComponentProperty>& GetProperties() const { return m_properties; }

    protected:

        /**
         * @brief Called when component is added to an object
         */
        virtual void Init() {}

        SceneObject* m_attachedObject = nullptr;
        const std::type_info& m_componentType;
        const std::wstring m_displayName;

        std::vector<ComponentProperty> m_properties;
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

// This is what we call ChatGPT magic
#define REGISTER_COMPONENT(TYPE) \
static bool TYPE##_registered = [](){ \
std::string type = #TYPE; \
Engine::ComponentRegister::Instance().Register(std::wstring(type.begin(), type.end()), []() { return std::make_unique<TYPE>(); }); \
return true; \
}()

}

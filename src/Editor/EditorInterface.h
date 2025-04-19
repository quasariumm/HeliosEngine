#pragma once
#include "Core/Window.h"

namespace Engine
{
class EditorInterface;

class EditorInterface
{
public:
    explicit EditorInterface(std::string name): name(std::move(name)) {}
    virtual ~EditorInterface() = default;

    virtual void DrawInterface() = 0;

    const std::string name;
    bool active = true;
};

class EditorInterfaceManager
{
public:
    EditorInterfaceManager() = default;

    static EditorInterfaceManager& Instance()
    {
        static EditorInterfaceManager instance;
        return instance;
    }

    void RegisterInterface(const std::string& name, std::unique_ptr<EditorInterface> interface) {
        m_editorInterfaces[name] = std::move(interface);
    }

    static void Initialize(Window* window);

    void DrawAllInterfaces() const;

    static void SetMouseEnabled(bool enable = true);
    static void SetKeyboardEnable(bool enable = true);

private:
    std::unordered_map<std::string, std::unique_ptr<EditorInterface>> m_editorInterfaces = {};
    Window* m_window = nullptr;
};

#define REGISTER_EDITOR_INTERFACE(TYPE) \
static bool TYPE##_registered = []() { \
Engine::EditorInterfaceManager::Instance().RegisterInterface(#TYPE, std::make_unique<TYPE>()); \
return true; \
}()

}


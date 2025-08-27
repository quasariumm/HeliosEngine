#pragma once
#include "Core/Window.h"

namespace Engine
{

enum class EditorInterfaceSource
{
    EDITOR,
    PROJECT
};

class EditorInterface
{
public:
    explicit EditorInterface(std::wstring name, const EditorInterfaceSource source = EditorInterfaceSource::PROJECT):
    name(std::move(name)), source(source) {}
    virtual ~EditorInterface() = default;

    virtual void DrawInterface() = 0;

    const std::wstring name;
    bool active = true;
    const EditorInterfaceSource source;
};

class EditorInterfaceManager
{
public:
    EditorInterfaceManager() = default;

    static ENGINE_API EditorInterfaceManager& Instance();

    void ENGINE_API RegisterInterface(const std::wstring& name, EditorInterface* editorInterface);

    void ENGINE_API UnregisterProjectInterfaces();

    static void Initialize(Window* window);

    void DrawAllInterfaces();

    static void SetMouseEnabled(bool enable = true);
    static void SetKeyboardEnable(bool enable = true);

private:
    std::unordered_map<std::wstring, EditorInterface*> m_editorInterfaces = {};
    Window* m_window = nullptr;
    std::unordered_map<std::wstring, std::filesystem::path> m_recentProjects = {};
};

#define REGISTER_EDITOR_INTERFACE(TYPE) \
static bool TYPE##_registered = []() { \
std::string type = #TYPE; \
Engine::EditorInterfaceManager::Instance().RegisterInterface(std::wstring(type.begin(), type.end()), new TYPE()); \
return true; \
}()

}


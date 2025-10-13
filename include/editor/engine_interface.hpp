#pragma once

struct ImFont;

namespace Engine::Editor
{

class EditorInterface
{
public:
    EditorInterface();
    ~EditorInterface();

    void StartFrame();
    void DrawInterfaces();
    void EndFrame();

    ImFont* GetCodeFont() const { return m_codeFont; }

    std::vector<std::function<void()>> interfaces;
    std::unordered_set<std::type_index> interfaceTypes;

    template<typename T>
    void RegisterInterface() { interfaces.push_back([]() { T::Draw(); }); }

    ImFont* m_codeFont;
};

template <typename T>
struct InterfaceHelper
{
    InterfaceHelper()
    {
        // Skip duplicates
        auto* types = &Systems::GetEditorInterface()->interfaceTypes;
        if (types->contains(std::type_index(typeid(T)))) return;
        
        Systems::GetEditorInterface()->interfaceTypes.insert(std::type_index(typeid(T)));
        
        Systems::GetEditorInterface()->RegisterInterface<T>();
    }
};

}

#define REGISTER_INTERFACE(TYPE) \
    namespace { Engine::Editor::InterfaceHelper<TYPE> reg_##TYPE; } \
    

#include "Component.h"

#include "Debugger/Debugger.h"

namespace Engine
{

ComponentRegister& ComponentRegister::Instance()
{
    static ComponentRegister instance;
    return instance;
}

void ComponentRegister::Register(const std::wstring& typeName, const std::wstring& displayName, Creator creator)
{
    m_registry[typeName] = std::move(creator);
    m_nameRegistry[typeName] = displayName;
    DebugLog(LogSeverity::INFO, L"Registered component: " + typeName);
}

}

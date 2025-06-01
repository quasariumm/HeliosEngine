#include "Core/FileTools.h"

#if defined(__GNUG__)
#include <cxxabi.h>
#include <cstdlib>
#endif

namespace Engine
{

std::string Demangle(const char* name)
{
#if defined(__GNUG__)
    int status = 0;
    char* demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result = (status == 0 && demangled != nullptr) ? demangled : name;
    free(demangled);
    // Strip namespaces: find last "::" and return what's after
    size_t pos = result.rfind("::");
    if (pos != std::string::npos)
    {
        return result.substr(pos + 2);
    }
    return result;
#else
            // MSVC or unsupported compiler — just return the raw name
            return name;
#endif
}
}


#include <iostream>
#include <windows.h>
#include <filesystem>

typedef int (__stdcall *engineFunc)();

int main()
{
    const std::wstring TMPFolder = std::filesystem::current_path().append("launcher\\cmake-build-debug");
	std::wstring EngineLibrary = std::filesystem::current_path().append("cmake-build-debug\\libEngined.dll");
	if (!std::filesystem::exists(EngineLibrary))
		EngineLibrary = std::filesystem::current_path().append("cmake-build-debug\\libEngine.dll");

    // Try to load the engine library
    if (std::filesystem::exists(TMPFolder + L"\\libEngine.dll"))
        std::filesystem::remove(TMPFolder + L"\\libEngine.dll");
    std::filesystem::copy(EngineLibrary, TMPFolder + L"\\libEngine.dll", std::filesystem::copy_options::overwrite_existing);

    const auto loadedLibrary = LoadLibraryW( (TMPFolder + L"\\libEngine.dll").c_str());

    if (!loadedLibrary) {
        std::cout << "could not load the dynamic library" << std::endl;
        return -1;
    }

    // resolve function address here
    const auto function = reinterpret_cast<engineFunc>(GetProcAddress(loadedLibrary, "main"));
    if (!function) {
        std::cout << "could not locate the function" << std::endl;
        FreeLibrary(loadedLibrary);
        return -1;
    }

    function();

    FreeLibrary(loadedLibrary);

    return 0;
}
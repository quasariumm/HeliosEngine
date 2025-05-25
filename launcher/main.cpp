#include <iostream>
#include <windows.h>
#include <filesystem>

typedef int (__stdcall *engineFunc)();

#define ENGINELIBRARY R"(C:\Users\Gebruiker\Documents\Projects\engine_raytrace\cmake-build-debug\libEngine.dll)"
#define TMPFOLDER R"(C:\Users\Gebruiker\Documents\Projects\engine_raytrace\launcher\Tmp)"

int main()
{
    // Try to load the engine library
    std::filesystem::copy(ENGINELIBRARY, TMPFOLDER"\\Engine.dll", std::filesystem::copy_options::overwrite_existing);

    HINSTANCE loadedLibrary = LoadLibrary( TMPFOLDER"\\Engine.dll");

    if (!loadedLibrary) {
        std::cout << "could not load the dynamic library" << std::endl;
        return -1;
    }

    // resolve function address here
    engineFunc function = (engineFunc)GetProcAddress(loadedLibrary, "main");
    if (!function) {
        std::cout << "could not locate the function" << std::endl;
        FreeLibrary(loadedLibrary);
        return -1;
    }

    function();

    FreeLibrary(loadedLibrary);

    return 0;
}
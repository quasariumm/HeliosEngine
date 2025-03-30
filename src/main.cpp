
#include "Backends/OpenGL46_GLFW/Core/GL46_Window.h"


int main(int argc, char* argv[])
{
    Engine::GL46_Window window{};
    window.Init({200, 100}, L"Varför är STL lokaler så irriterande?");



    return 0;
}

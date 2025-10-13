message(STATUS "Helios runs with the following API: OpenGL 4.6")
list(REMOVE_ITEM SOURCE ${DX12_BACKEND})
list(REMOVE_ITEM SOURCE ${VK_BACKEND})

file(GLOB IMGUI CONFIGURE_DEPENDS
		extern/glad/src/glad.c
		extern/imgui/backends/imgui_impl_opengl3.h
		extern/imgui/backends/imgui_impl_opengl3.cpp
		extern/imgui/backends/imgui_impl_opengl3_loader.h
		extern/imgui/backends/imgui_impl_glfw.h
		extern/imgui/backends/imgui_impl_glfw.cpp
)
list(APPEND LIBS ${IMGUI})
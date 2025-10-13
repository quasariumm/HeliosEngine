message(STATUS "Helios runs with the following API: DirectX 12")
list(REMOVE_ITEM SOURCE ${GL46_BACKEND})
list(REMOVE_ITEM SOURCE ${VK_BACKEND})

file(GLOB IMGUI CONFIGURE_DEPENDS
		extern/imgui/backends/imgui_impl_dx12.h
		extern/imgui/backends/imgui_impl_dx12.cpp
		extern/imgui/backends/imgui_impl_win32.h
		extern/imgui/backends/imgui_impl_win32.cpp
)
list(APPEND LIBS ${IMGUI})
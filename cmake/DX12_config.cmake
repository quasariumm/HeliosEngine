target_include_directories(Engine PRIVATE lib/directx)

target_compile_definitions(Engine PRIVATE "HELIOS_API_DX12=1")

# DX12 libraries
target_link_libraries(Engine PRIVATE
		d3d12.lib
		dxgi.lib
		dxguid.lib
		d3dcompiler.lib
		Dwmapi.lib
)
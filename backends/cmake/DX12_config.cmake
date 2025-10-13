target_include_directories(Engine PRIVATE lib/directx)

target_compile_definitions(Engine PRIVATE "HELIOS_API_DX12=1")
message(STATUS "Helios runs with the following API: Vulkan")
list(REMOVE_ITEM SOURCE ${GL46_BACKEND})
list(REMOVE_ITEM SOURCE ${DX12_BACKEND})
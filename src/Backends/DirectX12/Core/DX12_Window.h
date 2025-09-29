#pragma once
#include "Core/Window.h"

namespace Engine
{

class DX12_Window : public Window
{

public:

	DX12_Window() = default;

	bool Init(const vec2u& size, const std::wstring& title, uint32_t flags) override;

private:

	static void EnableDebugLayer();

	static bool RegisterWindowClass(HINSTANCE hInst, const wchar_t* windowClassName);

	bool CreateWindow(const wchar_t* windowClassName, HINSTANCE hInst, const wchar_t* windowTitle, vec2u size);

	static std::optional<ComPtr<IDXGIAdapter4>> GetAdapter(bool useWarp);

	bool CreateDevice(ComPtr<IDXGIAdapter4> adapter);

	bool CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type );

	static bool CheckTearingSupport();

	bool CreateSwapChain(HWND hWnd,
		ComPtr<ID3D12CommandQueue> commandQueue,
		uint32_t width, uint32_t height);

	bool CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type);

	bool CreateCommandAllocators(ComPtr<ID3D12Device2> device,
		D3D12_COMMAND_LIST_TYPE type);

	bool CreateCommandList(ComPtr<ID3D12Device2> device,
		ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type);

	bool CreateFence(ComPtr<ID3D12Device2> device);

	bool CreateEventHandle();

	void UpdateRenderTargetViews(ComPtr<ID3D12Device2> device,
		ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap);

	// Window handle.
	HWND m_hWnd;
	// Window rectangle (used to toggle fullscreen state).
	RECT m_windowRect;

	// Use WARP adapter
	bool m_useWarp = false;

	// DirectX 12 Objects
	ComPtr<ID3D12Device2> m_device;
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	ComPtr<IDXGISwapChain4> m_swapchain;
	ComPtr<ID3D12Resource> m_backBuffers[NUM_FRAMES];
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	ComPtr<ID3D12CommandAllocator> m_commandAllocators[NUM_FRAMES];
	ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
	UINT m_RTVDescriptorSize;
	UINT m_CurrentBackBufferIndex;

	// Synchronization objects
	ComPtr<ID3D12Fence> m_fence;
	uint64_t m_fenceValue = 0;
	uint64_t m_frameFenceValues[NUM_FRAMES] = {};
	HANDLE m_fenceEvent;

	bool m_tearingSupported = false;

};

}
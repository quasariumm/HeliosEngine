#include "DX12_Window.h"

#include "Debugger/Debugger.h"

using namespace Engine;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void DX12_Window::EnableDebugLayer()
{
#ifdef _DEBUG
	// Always enable the debug layer before doing anything DX12 related
	// so all possible errors generated while creating DX12 objects
	// are caught by the debug layer.
	ComPtr<ID3D12Debug> debugInterface;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)));
	debugInterface->EnableDebugLayer();
#endif
}

bool DX12_Window::RegisterWindowClass( HINSTANCE hInst, const wchar_t* windowClassName )
{
	// Register a window class for creating our render window with.
	WNDCLASSEXW windowClass = {};

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = &WndProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = hInst;
	windowClass.hIcon = ::LoadIcon(hInst, NULL);
	windowClass.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = ::LoadIcon(hInst, NULL);

	static ATOM atom = ::RegisterClassExW(&windowClass);
	return atom > 0;
}

bool DX12_Window::CreateWindow(const wchar_t* windowClassName, HINSTANCE hInst, const wchar_t* windowTitle, vec2u size)
{
	int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

	RECT windowRect = { 0, 0, static_cast<LONG>(size.x), static_cast<LONG>(size.y) };
	::AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	// Center the window within the screen. Clamp to 0, 0 for the top-left corner.
	int windowX = std::max<int>(0, (screenWidth - windowWidth) / 2);
	int windowY = std::max<int>(0, (screenHeight - windowHeight) / 2);
	HWND hWnd = ::CreateWindowExW(
		NULL,
		windowClassName,
		windowTitle,
		WS_OVERLAPPEDWINDOW,
		windowX,
		windowY,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		hInst,
		nullptr
	);

	if (!hWnd)
		return false;

	m_hWnd = hWnd;
	return true;
}

std::optional<ComPtr<IDXGIAdapter4>> DX12_Window::GetAdapter(bool useWarp)
{
	ComPtr<IDXGIFactory4> dxgiFactory;
	UINT createFactoryFlags = 0;
#ifdef _DEBUG
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (!SUCCEEDED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory))))
		return {};
	ComPtr<IDXGIAdapter1> dxgiAdapter1;
	ComPtr<IDXGIAdapter4> dxgiAdapter4;

	if (useWarp)
	{
		if (!SUCCEEDED(dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter1))))
			return {};
		if (!SUCCEEDED(dxgiAdapter1.As(&dxgiAdapter4)))
			return {};
	}
	else
	{
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 dxgiAdapterDesc1;
			dxgiAdapter1->GetDesc1(&dxgiAdapterDesc1);

			// Check to see if the adapter can create a D3D12 device without actually
			// creating it. The adapter with the largest dedicated video memory
			// is favored.
			if ((dxgiAdapterDesc1.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(D3D12CreateDevice(dxgiAdapter1.Get(),
					D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
				dxgiAdapterDesc1.DedicatedVideoMemory > maxDedicatedVideoMemory )
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc1.DedicatedVideoMemory;
				if (!SUCCEEDED(dxgiAdapter1.As(&dxgiAdapter4)))
					return {};
			}
		}
	}

	return dxgiAdapter4;
}

bool DX12_Window::CreateDevice(ComPtr<IDXGIAdapter4> adapter)
{
	if (!SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device))))
		return false;
	// Enable debug messages in debug mode.
#ifdef _DEBUG
	ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(m_device.As(&pInfoQueue)))
	{
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		//NewFilter.DenyList.NumCategories = _countof(Categories);
		//NewFilter.DenyList.pCategoryList = Categories;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;

		if (!SUCCEEDED(pInfoQueue->PushStorageFilter(&NewFilter)))
			return false;
	}
#endif
	return true;
}

bool DX12_Window::CreateCommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type )
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type =     type;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags =    D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	return SUCCEEDED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));
}

bool DX12_Window::CheckTearingSupport()
{
	BOOL allowTearing = FALSE;

	// Rather than create the DXGI 1.5 factory interface directly, we create the
	// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the
	// graphics debugging tools which will not support the 1.5 factory interface
	// until a future update.
	ComPtr<IDXGIFactory4> factory4;
	if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
	{
		ComPtr<IDXGIFactory5> factory5;
		if (SUCCEEDED(factory4.As(&factory5)))
		{
			if (FAILED(factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing, sizeof(allowTearing))))
			{
				allowTearing = FALSE;
			}
		}
	}

	return allowTearing == TRUE;
}


bool DX12_Window::CreateSwapChain(HWND hWnd,
	ComPtr<ID3D12CommandQueue> commandQueue,
	uint32_t width, uint32_t height )
{
	ComPtr<IDXGIFactory4> dxgiFactory4;
	UINT createFactoryFlags = 0;
#ifdef _DEBUG
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (!SUCCEEDED(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4))))
		return false;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = width;
	swapChainDesc.Height = height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = NUM_FRAMES;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	// It is recommended to always allow tearing if tearing support is available.
	swapChainDesc.Flags = CheckTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
	ComPtr<IDXGISwapChain1> swapChain1;
	if (!SUCCEEDED(dxgiFactory4->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hWnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1)))
		return false;

	// Disable the Alt+Enter fullscreen toggle feature. Switching to fullscreen
	// will be handled manually.
	if (!SUCCEEDED(dxgiFactory4->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER)))
		return false;

	return SUCCEEDED(swapChain1.As(&m_swapchain));
}


bool DX12_Window::CreateDescriptorHeap(ComPtr<ID3D12Device2> device, D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = NUM_FRAMES;
	desc.Type = type;

	return SUCCEEDED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_RTVDescriptorHeap)));
}


void DX12_Window::UpdateRenderTargetViews(ComPtr<ID3D12Device2> device,
	ComPtr<IDXGISwapChain4> swapChain, ComPtr<ID3D12DescriptorHeap> descriptorHeap)
{
	auto rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(descriptorHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < NUM_FRAMES; ++i)
	{
		ComPtr<ID3D12Resource> backBuffer;
		if (!SUCCEEDED(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
		{
			DebugLog(LogSeverity::SEVERE, std::format(L"Failed to get buffer from swapchain! Idx: {}", i));
			return;
		}

		device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

		m_backBuffers[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}
}


bool DX12_Window::CreateCommandAllocators(ComPtr<ID3D12Device2> device,
	D3D12_COMMAND_LIST_TYPE type)
{
	for (int i = 0; i < NUM_FRAMES; ++i)
		if (!SUCCEEDED(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_commandAllocators[i]))))
			return false;
	return true;
}


bool DX12_Window::CreateCommandList(ComPtr<ID3D12Device2> device,
	ComPtr<ID3D12CommandAllocator> commandAllocator, D3D12_COMMAND_LIST_TYPE type)
{
	if (!SUCCEEDED(device->CreateCommandList(
		0, type, commandAllocator.Get(),
		nullptr, IID_PPV_ARGS(&m_commandList))))
		return false;

	if (!SUCCEEDED(m_commandList->Close()))
		return false;
	return true;
}


bool DX12_Window::CreateFence(ComPtr<ID3D12Device2> device)
{
	return SUCCEEDED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
}


bool DX12_Window::CreateEventHandle()
{
	m_fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	return m_fenceEvent != nullptr;
}


// uint64_t DX12_Window::Signal(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
// 	uint64_t& fenceValue)
// {
// 	uint64_t fenceValueForSignal = ++fenceValue;
// 	ThrowIfFailed(commandQueue->Signal(fence.Get(), fenceValueForSignal));
//
// 	return fenceValueForSignal;
// }
//
//
// void DX12_Window::WaitForFenceValue(ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent,
// 	std::chrono::milliseconds duration = std::chrono::milliseconds::max() )
// {
// 	if (fence->GetCompletedValue() < fenceValue)
// 	{
// 		ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
// 		::WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
// 	}
// }
//
//
// void DX12_Window::Flush(ComPtr<ID3D12CommandQueue> commandQueue, ComPtr<ID3D12Fence> fence,
// 	uint64_t& fenceValue, HANDLE fenceEvent )
// {
// 	uint64_t fenceValueForSignal = Signal(commandQueue, fence, fenceValue);
// 	WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
// }
//
//
// void DX12_Window::Update()
// {
// 	static uint64_t frameCounter = 0;
// 	static double elapsedSeconds = 0.0;
// 	static std::chrono::high_resolution_clock clock;
// 	static auto t0 = std::chrono::high_resolution_clock::now();
//
// 	frameCounter++;
// 	auto t1 = std::chrono::high_resolution_clock::now();
// 	auto deltaTime = t1 - t0;
// 	t0 = t1;
// 	elapsedSeconds += deltaTime.count() * 1e-9;
// 	if (elapsedSeconds > 1.0)
// 	{
// 		char buffer[500];
// 		auto fps = frameCounter / elapsedSeconds;
// 		sprintf_s(buffer, 500, "FPS: %f\n", fps);
// 		OutputDebugString(buffer);
//
// 		frameCounter = 0;
// 		elapsedSeconds = 0.0;
// 	}
// }
//
//
// void DX12_Window::Render()
// {
// 	auto commandAllocator = m_commandAllocators[m_CurrentBackBufferIndex];
// 	auto backBuffer = m_backBuffers[m_CurrentBackBufferIndex];
//
// 	commandAllocator->Reset();
// 	m_commandList->Reset(commandAllocator.Get(), nullptr);
// 	// Clear the render target.
// 	{
// 		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
// 			backBuffer.Get(),
// 			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
//
// 		m_commandList->ResourceBarrier(1, &barrier);
// 		FLOAT clearColor[] = { 0.4f, 0.6f, 0.9f, 1.0f };
// 		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
// 			m_CurrentBackBufferIndex, m_RTVDescriptorSize);
//
// 		m_commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
// 	}
// 	// Present
// 	{
// 		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
// 			backBuffer.Get(),
// 			D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
// 		m_commandList->ResourceBarrier(1, &barrier);
// 		ThrowIfFailed(m_commandList->Close());
//
// 		ID3D12CommandList* const commandLists[] = {
// 			m_commandList.Get()
// 		};
// 		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
// 		UINT syncInterval = g_VSync ? 1 : 0;
// 		UINT presentFlags = m_tearingSupported && !g_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;
// 		ThrowIfFailed(m_swapchain->Present(syncInterval, presentFlags));
//
// 		m_frameFenceValues[m_CurrentBackBufferIndex] = Signal(m_commandQueue, m_fence, m_fenceValue);
// 		m_CurrentBackBufferIndex = m_swapchain->GetCurrentBackBufferIndex();
//
// 		WaitForFenceValue(m_fence, m_frameFenceValues[m_CurrentBackBufferIndex], m_fenceEvent);
// 	}
// }
//
//
// void DX12_Window::Resize(uint32_t width, uint32_t height)
// {
// 	if (g_ClientWidth != width || g_ClientHeight != height)
// 	{
// 		// Don't allow 0 size swap chain back buffers.
// 		g_ClientWidth = std::max(1u, width );
// 		g_ClientHeight = std::max( 1u, height);
//
// 		// Flush the GPU queue to make sure the swap chain's back buffers
// 		// are not being referenced by an in-flight command list.
// 		Flush(m_commandQueue, m_fence, m_fenceValue, m_fenceEvent);
// 		for (int i = 0; i < g_NumFrames; ++i)
// 		{
// 			// Any references to the back buffers must be released
// 			// before the swap chain can be resized.
// 			m_backBuffers[i].Reset();
// 			m_frameFenceValues[i] = m_frameFenceValues[m_CurrentBackBufferIndex];
// 		}
// 		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
// 		ThrowIfFailed(m_swapchain->GetDesc(&swapChainDesc));
// 		ThrowIfFailed(m_swapchain->ResizeBuffers(g_NumFrames, g_ClientWidth, g_ClientHeight,
// 			swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));
//
// 		m_CurrentBackBufferIndex = m_swapchain->GetCurrentBackBufferIndex();
//
// 		UpdateRenderTargetViews(m_device, m_swapchain, m_RTVDescriptorHeap);
// 	}
// }
//
//
// void DX12_Window::SetFullscreen(bool fullscreen)
// {
// 	if (g_Fullscreen != fullscreen)
// 	{
// 		g_Fullscreen = fullscreen;
//
// 		if (g_Fullscreen) // Switching to fullscreen.
// 		{
// 			// Store the current window dimensions so they can be restored
// 			// when switching out of fullscreen state.
// 			::GetWindowRect(m_hWnd, &m_windowRect);
// 			// Set the window style to a borderless window so the client area fills
// 			// the entire screen.
// 			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
//
// 			::SetWindowLongW(m_hWnd, GWL_STYLE, windowStyle);
// 			// Query the name of the nearest display device for the window.
// 			// This is required to set the fullscreen dimensions of the window
// 			// when using a multi-monitor setup.
// 			HMONITOR hMonitor = ::MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
// 			MONITORINFOEX monitorInfo = {};
// 			monitorInfo.cbSize = sizeof(MONITORINFOEX);
// 			::GetMonitorInfo(hMonitor, &monitorInfo);
// 			::SetWindowPos(m_hWnd, HWND_TOP,
// 	monitorInfo.rcMonitor.left,
// 	monitorInfo.rcMonitor.top,
// 	monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
// 	monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
// 	SWP_FRAMECHANGED | SWP_NOACTIVATE);
//
// 			::ShowWindow(m_hWnd, SW_MAXIMIZE);
// 		}
// 		else
// 		{
// 			// Restore all the window decorators.
// 			::SetWindowLong(m_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
//
// 			::SetWindowPos(m_hWnd, HWND_NOTOPMOST,
// 				m_windowRect.left,
// 				m_windowRect.top,
// 				m_windowRect.right - m_windowRect.left,
// 				m_windowRect.bottom - m_windowRect.top,
// 				SWP_FRAMECHANGED | SWP_NOACTIVATE);
//
// 			::ShowWindow(m_hWnd, SW_NORMAL);
// 		}
// 	}
// }


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// if ( m_initialized )
	// {
	// 	switch (message)
	// 	{
	// 	case WM_PAINT:
	// 		Update();
	// 		Render();
	// 		break;
	// 	case WM_SYSKEYDOWN:
	// 	case WM_KEYDOWN:
	// 	{
	// 		bool alt = (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
	//
	// 		switch (wParam)
	// 		{
	// 		case 'V':
	// 			m_vsync = !m_vsync;
	// 			break;
	// 		case VK_ESCAPE:
	// 			::PostQuitMessage(0);
	// 			break;
	// 		case VK_RETURN:
	// 			if ( alt )
	// 			{
	// 				case VK_F11:
	// 				SetFullscreen(!m_fullscreen);
	// 			}
	// 			break;
	// 		}
	// 	}
	// 		break;
	// 		// The default window procedure will play a system notification sound
	// 		// when pressing the Alt+Enter keyboard combination if this message is
	// 		// not handled.
	// 	case WM_SYSCHAR:
	// 		break;
	// 	case WM_SIZE:
	// 	{
	// 		RECT clientRect = {};
	// 		::GetClientRect(g_hWnd, &clientRect);
	//
	// 		int width = clientRect.right - clientRect.left;
	// 		int height = clientRect.bottom - clientRect.top;
	//
	// 		Resize(width, height);
	// 	}
	// 		break;
	// 	case WM_DESTROY:
	// 		::PostQuitMessage(0);
	// 		break;
	// 	default:
	// 		return ::DefWindowProcW(hwnd, message, wParam, lParam);
	// 	}
	// }
	// else
	// {
	// 	return ::DefWindowProcW(hwnd, message, wParam, lParam);
	// }

	return 0;
}


bool DX12_Window::Init( const vec2u& size, const std::wstring& title, uint32_t flags )
{
	if (m_initialized)
	{
		DebugLog(LogSeverity::SEVERE, L"Window already initialised! Returning...");
		return false;
	}
	m_title = title;
	// Windows 10 Creators update adds Per Monitor V2 DPI awareness context.
	// Using this awareness context allows the client area of the window
	// to achieve 100% scaling while still allowing non-client window content to
	// be rendered in a DPI sensitive fashion.
	// SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	// Window class name. Used for registering / creating the window.
	const wchar_t* windowClassName = L"DX12WindowClass";

	EnableDebugLayer();

	if (!RegisterWindowClass(HINST_THISCOMPONENT, windowClassName))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to register window class.");
		return false;
	}

	if (!CreateWindow(windowClassName, HINST_THISCOMPONENT, title.c_str(), size))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create window.");
		return false;
	}

	// Get the window rect
	::GetWindowRect(m_hWnd, &m_windowRect);

	const auto adapterRes = GetAdapter(m_useWarp);
	if (!adapterRes.has_value())
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to get adapter.");
		return false;
	}
	const ComPtr<IDXGIAdapter4> dxgiAdapter4 = adapterRes.value();

	if (!CreateDevice(dxgiAdapter4))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create device.");
		return false;
	}

	if (!CreateCommandQueue(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create command queue.");
		return false;
	}

	if (!CreateSwapChain(m_hWnd, m_commandQueue, size.x, size.y))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create swap chain.");
		return false;
	}

	m_CurrentBackBufferIndex = m_swapchain->GetCurrentBackBufferIndex();

	if (!CreateDescriptorHeap(m_device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create descriptor heap.");
		return false;
	}

	m_RTVDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	UpdateRenderTargetViews(m_device, m_swapchain, m_RTVDescriptorHeap);

	if (!CreateCommandAllocators(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create command allocators.");
		return false;
	}

	if (!CreateCommandList(m_device, m_commandAllocators[m_CurrentBackBufferIndex], D3D12_COMMAND_LIST_TYPE_DIRECT))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create command list.");
		return false;
	}

	if (!CreateFence(m_device))
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create fence.");
		return false;
	}

	if (!CreateEventHandle())
	{
		DebugLog(LogSeverity::SEVERE, L"Failed to create event handle.");
		return false;
	}

	m_initialized = true;

	::ShowWindow(m_hWnd, SW_SHOW);

	return true;
}

#pragma once

namespace Engine
{

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)

// The number of swap chain back buffers.
static constexpr uint8_t NUM_FRAMES = 3;

// From DXSampleHelper.h
// Source: https://github.com/Microsoft/DirectX-Graphics-Samples
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw std::exception();
	}
}

}

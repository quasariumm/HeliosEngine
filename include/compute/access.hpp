#pragma once

namespace Engine::Compute
{

enum Access : uint16_t
{
	Access_DEVICE_READ_WRITE     = (1 << 0),
	Access_DEVICE_WRITE_ONLY     = (1 << 1),
	Access_DEVICE_READ_ONLY      = (1 << 2),
	Access_DEVICE_USE_HOST_PTR   = (1 << 3),
	Access_DEVICE_ALLOC_HOST_PTR = (1 << 4),
	Access_DEVICE_COPY_HOST_PTR  = (1 << 5),
	Access_HOST_WRITE_ONLY       = (1 << 7),
	Access_HOST_READ_ONLY        = (1 << 8),
	Access_HOST_NO_ACCESS        = (1 << 9),

	// Common use-cases
	Access_COPIED_READ_ONLY	   = Access_DEVICE_READ_ONLY | Access_DEVICE_COPY_HOST_PTR,
	Access_ALLOCED_READ_ONLY   = Access_DEVICE_READ_ONLY | Access_DEVICE_ALLOC_HOST_PTR,
};

using access_flag_t = uint16_t;

}

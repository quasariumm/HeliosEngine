#pragma once

#pragma region OpenCL type trait
// Helper trait to detect OpenCL vector types
template <typename T>
struct is_cl_vector : std::false_type {};

template <> struct is_cl_vector<cl_float2> : std::true_type {};
template <> struct is_cl_vector<cl_float3> : std::true_type {};
template <> struct is_cl_vector<cl_float8> : std::true_type {};
template <> struct is_cl_vector<cl_float16> : std::true_type {};

template <> struct is_cl_vector<cl_double2> : std::true_type {};
template <> struct is_cl_vector<cl_double3> : std::true_type {};
template <> struct is_cl_vector<cl_double8> : std::true_type {};
template <> struct is_cl_vector<cl_double16> : std::true_type {};

template <> struct is_cl_vector<cl_int2> : std::true_type {};
template <> struct is_cl_vector<cl_int3> : std::true_type {};
template <> struct is_cl_vector<cl_int8> : std::true_type {};
template <> struct is_cl_vector<cl_int16> : std::true_type {};

template <> struct is_cl_vector<cl_uint2> : std::true_type {};
template <> struct is_cl_vector<cl_uint3> : std::true_type {};
template <> struct is_cl_vector<cl_uint8> : std::true_type {};
template <> struct is_cl_vector<cl_uint16> : std::true_type {};

template <> struct is_cl_vector<cl_long2> : std::true_type {};
template <> struct is_cl_vector<cl_long3> : std::true_type {};
template <> struct is_cl_vector<cl_long8> : std::true_type {};
template <> struct is_cl_vector<cl_long16> : std::true_type {};

template <> struct is_cl_vector<cl_ulong2> : std::true_type {};
template <> struct is_cl_vector<cl_ulong3> : std::true_type {};
template <> struct is_cl_vector<cl_ulong8> : std::true_type {};
template <> struct is_cl_vector<cl_ulong16> : std::true_type {};

template <> struct is_cl_vector<cl_short2> : std::true_type {};
template <> struct is_cl_vector<cl_short3> : std::true_type {};
template <> struct is_cl_vector<cl_short8> : std::true_type {};
template <> struct is_cl_vector<cl_short16> : std::true_type {};

template <> struct is_cl_vector<cl_ushort2> : std::true_type {};
template <> struct is_cl_vector<cl_ushort3> : std::true_type {};
template <> struct is_cl_vector<cl_ushort8> : std::true_type {};
template <> struct is_cl_vector<cl_ushort16> : std::true_type {};

template <> struct is_cl_vector<cl_char2> : std::true_type {};
template <> struct is_cl_vector<cl_char3> : std::true_type {};
template <> struct is_cl_vector<cl_char8> : std::true_type {};
template <> struct is_cl_vector<cl_char16> : std::true_type {};

template <> struct is_cl_vector<cl_uchar2> : std::true_type {};
template <> struct is_cl_vector<cl_uchar3> : std::true_type {};
template <> struct is_cl_vector<cl_uchar8> : std::true_type {};
template <> struct is_cl_vector<cl_uchar16> : std::true_type {};

template <typename T>
inline constexpr bool is_cl_vector_v = is_cl_vector<T>::value;
#pragma endregion

#pragma region GLM to OpenCL vector and matrix conversion
// Vector conversions
inline cl_float2 glmToCL(const glm::vec2& v) { return {v.x, v.y}; }
inline cl_float3 glmToCL(const glm::vec3& v) { return {v.x, v.y, v.z}; }
inline cl_float4 glmToCL(const glm::vec4& v) { return {v.x, v.y, v.z, v.w}; }

// Integer vector conversions
inline cl_int2 glmToCL(const glm::ivec2& v) { return {v.x, v.y}; }
inline cl_int3 glmToCL(const glm::ivec3& v) { return {v.x, v.y, v.z}; }
inline cl_int4 glmToCL(const glm::ivec4& v) { return {v.x, v.y, v.z, v.w}; }

// Unsigned integer vector conversions
inline cl_uint2 glmToCL(const glm::uvec2& v) { return {v.x, v.y}; }
inline cl_uint3 glmToCL(const glm::uvec3& v) { return {v.x, v.y, v.z}; }
inline cl_uint4 glmToCL(const glm::uvec4& v) { return {v.x, v.y, v.z, v.w}; }

// Matrix conversions to float16 (column-major, matching OpenCL)
inline cl_float16 glmToCL(const glm::mat4& m) {
	return {
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]
	};
}

inline cl_float16 glmToCL(const glm::mat3& m) {
	return {
		m[0][0], m[0][1], m[0][2], 0.0f,
		m[1][0], m[1][1], m[1][2], 0.0f,
		m[2][0], m[2][1], m[2][2], 0.0f,
		0.0f,    0.0f,    0.0f,    1.0f
	};
}

inline cl_float16 glmToCL(const glm::mat2& m) {
	return {
		m[0][0], m[0][1], 0.0f, 0.0f,
		m[1][0], m[1][1], 0.0f, 0.0f,
		0.0f,    0.0f,    1.0f, 0.0f,
		0.0f,    0.0f,    0.0f, 1.0f
	};
}
#pragma endregion

inline std::string CLErrorString( const cl_int result )
{
	switch (result)
	{
	case CL_DEVICE_NOT_FOUND:
		return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:
		return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:
		return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:
		return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:
		return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:
		return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:
		return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:
		return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:
		return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:
		return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:
		return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:
		return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:
		return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:
		return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case CL_INVALID_VALUE:
		return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:
		return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:
		return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:
		return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:
		return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:
		return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:
		return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:
		return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:
		return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
		return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:
		return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:
		return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:
		return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:
		return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:
		return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:
		return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:
		return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:
		return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:
		return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:
		return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:
		return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:
		return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:
		return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:
		return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:
		return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:
		return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:
		return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:
		return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:
		return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:
		return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:
		return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:
		return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:
		return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:
		return "CL_INVALID_GLOBAL_WORK_SIZE";
	case CL_INVALID_PROPERTY:
		return "CL_INVALID_PROPERTY";
	case CL_INVALID_IMAGE_DESCRIPTOR:
		return "CL_INVALID_IMAGE_DESCRIPTOR";
	case CL_INVALID_COMPILER_OPTIONS:
		return "CL_INVALID_COMPILER_OPTIONS";
	case CL_INVALID_LINKER_OPTIONS:
		return "CL_INVALID_LINKER_OPTIONS";
	case CL_INVALID_DEVICE_PARTITION_COUNT:
		return "CL_INVALID_DEVICE_PARTITION_COUNT";
	default:
		return "CL_INVALID_VALUE";
	}
}
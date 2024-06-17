#pragma once
#include <memory>

#ifdef PE_PLATFORM_WINDOWS
	#ifdef PE_BUILD_DLL
		#define PINE_API __declspec(dllexport)
	#else
		#define PINE_API __declspec(dllimport)
	#endif
#else
	#error Pine only supprt windows!
#endif

#ifdef PE_DEBUG
	#if defined(PE_PLATFORM_WINDOWS)
		#define PE_DEBUGBREAK() __debugbreak()
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
		#define PE_ENABLE_ASSERTS
#else
	#define PE_DEBUGBREAK()
#endif

#define BIT(x) (1 << x)
#define PE_BIND_EVENT_FN(fn, owner) std::bind(&fn, owner, std::placeholders::_1)
#define PE_EXPAND_MACRO(x) x
#define PE_STRINGIFY_MACRO(x) #x

namespace Pine
{

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;


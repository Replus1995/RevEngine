#pragma once
#include <memory>

#ifdef RE_PLATFORM_WINDOWS
	#ifdef RE_BUILD_DLL
		#define REV_API __declspec(dllexport)
	#else
		#define REV_API __declspec(dllimport)
	#endif
#else
	#error Rev only supprt windows!
#endif

#ifdef RE_DEBUG
	#if defined(RE_PLATFORM_WINDOWS)
		#define RE_DEBUGBREAK() __debugbreak()
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
		#define RE_ENABLE_ASSERTS
#else
	#define RE_DEBUGBREAK()
#endif

#define BIT(x) (1 << x)
#define RE_BIND_EVENT_FN(fn, owner) std::bind(&fn, owner, std::placeholders::_1)
#define RE_EXPAND_MACRO(x) x
#define RE_STRINGIFY_MACRO(x) #x

namespace Rev
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

//#ifndef GLM_ENABLE_EXPERIMENTAL
//	#define GLM_ENABLE_EXPERIMENTAL
//#endif // !GLM_ENABLE_EXPERIMENTAL

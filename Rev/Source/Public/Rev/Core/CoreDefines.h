#pragma once
#ifdef REV_PLATFORM_WINDOWS
#ifdef REV_BUILD_DLL
#define REV_API __declspec(dllexport)
#else
#define REV_API __declspec(dllimport)
#endif
#else
#error Rev only supprt windows!
#endif

#ifdef RE_DEBUG
#if defined(REV_PLATFORM_WINDOWS)
#define RE_DEBUGBREAK() __debugbreak()
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define RE_ENABLE_ASSERTS
#else
#define RE_DEBUGBREAK()
#endif

#ifndef FORCEINLINE
#ifdef _MSC_VER
#if(_MSC_VER >= 1200)
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE __inline
#endif
#else
#define FORCEINLINE
#endif // _MSC_VER
#endif

#ifndef FORCENOINLINE
#ifdef _MSC_VER
#define FORCENOINLINE __declspec(noinline)
#else
#define FORCENOINLINE
#endif
#endif

#ifndef DEPRECATED
#ifdef _MSC_VER
#define DEPRECATED(...) __declspec(deprecated(__VA_ARGS__))
#else
#define DEPRECATED(...)
#endif
#endif

#define BIT(x) (1 << x)
#define RE_EXPAND_MACRO(x) x
#define RE_STRINGIFY_MACRO(x) #x
#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Log.h"
#include <filesystem>

#ifdef REV_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define REV_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { REV##type##ERROR(msg, __VA_ARGS__); REV_DEBUGBREAK(); } }
	#define REV_INTERNAL_ASSERT_WITH_MSG(type, check, ...) REV_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define REV_INTERNAL_ASSERT_NO_MSG(type, check) REV_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", REV_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define REV_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define REV_INTERNAL_ASSERT_GET_MACRO(...) REV_EXPAND_MACRO( REV_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, REV_INTERNAL_ASSERT_WITH_MSG, REV_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define REV_ASSERT(...) REV_EXPAND_MACRO( REV_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define REV_CORE_ASSERT(...) REV_EXPAND_MACRO( REV_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define REV_ASSERT(...)
	#define REV_CORE_ASSERT(...)
#endif
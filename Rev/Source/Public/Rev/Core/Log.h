#pragma once
#include "Rev/Core/Base.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Rev
{
	class REV_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return sCoreLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return sClientLogger; };

	private:
		static std::shared_ptr<spdlog::logger> sCoreLogger;
		static std::shared_ptr<spdlog::logger> sClientLogger;
	};
}

//Core log marcos
#define RE_CORE_TRACE(...)		::Rev::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RE_CORE_INFO(...)		::Rev::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RE_CORE_WARN(...)		::Rev::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RE_CORE_ERROR(...)		::Rev::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RE_CORE_CRITICAL(...)	::Rev::Log::GetCoreLogger()->critical(__VA_ARGS__)

//Client log marcos
#define RE_TRACE(...)			::Rev::Log::GetClientLogger()->trace(__VA_ARGS__)
#define RE_INFO(...)			::Rev::Log::GetClientLogger()->info(__VA_ARGS__)
#define RE_WARN(...)			::Rev::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RE_ERROR(...)			::Rev::Log::GetClientLogger()->error(__VA_ARGS__)
#define RE_CRITICAL(...)		::Rev::Log::GetClientLogger()->critical(__VA_ARGS__)
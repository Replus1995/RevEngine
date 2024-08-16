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
#define REV_CORE_TRACE(...)		::Rev::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define REV_CORE_VERBOSE(...)	::Rev::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define REV_CORE_INFO(...)		::Rev::Log::GetCoreLogger()->info(__VA_ARGS__)
#define REV_CORE_WARN(...)		::Rev::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define REV_CORE_ERROR(...)		::Rev::Log::GetCoreLogger()->error(__VA_ARGS__)
#define REV_CORE_CRITICAL(...)	::Rev::Log::GetCoreLogger()->critical(__VA_ARGS__)

//Client log marcos
#define REV_TRACE(...)			::Rev::Log::GetClientLogger()->trace(__VA_ARGS__)
#define REV_VERBOSE(...)		::Rev::Log::GetClientLogger()->debug(__VA_ARGS__)
#define REV_INFO(...)			::Rev::Log::GetClientLogger()->info(__VA_ARGS__)
#define REV_WARN(...)			::Rev::Log::GetClientLogger()->warn(__VA_ARGS__)
#define REV_ERROR(...)			::Rev::Log::GetClientLogger()->error(__VA_ARGS__)
#define REV_CRITICAL(...)		::Rev::Log::GetClientLogger()->critical(__VA_ARGS__)
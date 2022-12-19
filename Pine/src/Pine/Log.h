#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"

namespace Pine
{
	class PINE_API Log
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
#define PINE_CORE_TRACE(...)	::Pine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define PINE_CORE_INFO(...)		::Pine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define PINE_CORE_WARN(...)		::Pine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define PINE_CORE_ERROR(...)	::Pine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define PINE_CORE_FATAL(...)	::Pine::Log::GetCoreLogger()->critical(__VA_ARGS__)

//Client log marcos
#define PINE_TRACE(...)			::Pine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define PINE_INFO(...)			::Pine::Log::GetClientLogger()->info(__VA_ARGS__)
#define PINE_WARN(...)			::Pine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define PINE_ERROR(...)			::Pine::Log::GetClientLogger()->error(__VA_ARGS__)
#define PINE_FATAL(...)			::Pine::Log::GetClientLogger()->critical(__VA_ARGS__)
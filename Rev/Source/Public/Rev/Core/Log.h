#pragma once
#include "Rev/Core/Base.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Rev
{

class REV_API FLogger
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
#define REV_CORE_TRACE(...)		::Rev::FLogger::GetCoreLogger()->trace(__VA_ARGS__)
#define REV_CORE_VERBOSE(...)	::Rev::FLogger::GetCoreLogger()->debug(__VA_ARGS__)
#define REV_CORE_INFO(...)		::Rev::FLogger::GetCoreLogger()->info(__VA_ARGS__)
#define REV_CORE_WARN(...)		::Rev::FLogger::GetCoreLogger()->warn(__VA_ARGS__)
#define REV_CORE_ERROR(...)		::Rev::FLogger::GetCoreLogger()->error(__VA_ARGS__)
#define REV_CORE_CRITICAL(...)	::Rev::FLogger::GetCoreLogger()->critical(__VA_ARGS__)

//Client log marcos
#define REV_TRACE(...)			::Rev::FLogger::GetClientLogger()->trace(__VA_ARGS__)
#define REV_VERBOSE(...)		::Rev::FLogger::GetClientLogger()->debug(__VA_ARGS__)
#define REV_INFO(...)			::Rev::FLogger::GetClientLogger()->info(__VA_ARGS__)
#define REV_WARN(...)			::Rev::FLogger::GetClientLogger()->warn(__VA_ARGS__)
#define REV_ERROR(...)			::Rev::FLogger::GetClientLogger()->error(__VA_ARGS__)
#define REV_CRITICAL(...)		::Rev::FLogger::GetClientLogger()->critical(__VA_ARGS__)
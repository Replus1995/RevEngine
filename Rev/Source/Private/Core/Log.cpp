#include "Rev/Core/Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Rev
{

std::shared_ptr<spdlog::logger> FLogger::sCoreLogger{ nullptr };
std::shared_ptr<spdlog::logger> FLogger::sClientLogger{ nullptr };

void FLogger::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	sCoreLogger = spdlog::stdout_color_mt("REV");
	sCoreLogger->set_level(spdlog::level::trace);

	sClientLogger = spdlog::stdout_color_mt("APP");
	sClientLogger->set_level(spdlog::level::trace);

}

}
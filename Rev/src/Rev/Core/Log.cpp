#include "Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace Rev
{
	std::shared_ptr<spdlog::logger> Log::sCoreLogger{ nullptr };
	std::shared_ptr<spdlog::logger> Log::sClientLogger{ nullptr };

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v%$");
		sCoreLogger = spdlog::stdout_color_mt("PINE");
		sCoreLogger->set_level(spdlog::level::trace);

		sClientLogger = spdlog::stdout_color_mt("APP");
		sClientLogger->set_level(spdlog::level::trace);

	}
}
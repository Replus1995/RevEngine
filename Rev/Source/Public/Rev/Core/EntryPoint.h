#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Log.h"

#ifdef RE_PLATFORM_WINDOWS

namespace Rev
{
	extern Application* CreateApplication();

	inline int RunApp(int argc, char** argv)
	{
		Log::Init();
		RE_CORE_WARN("Initialized Log!");

		auto app = Rev::CreateApplication();
		app->Run();
		delete app;

		return 0;
	}
}
#endif

#pragma once
#include "Pine/Core/Base.h"
#include "Pine/Core/Application.h"
#include "Pine/Core/Log.h"

#ifdef PE_PLATFORM_WINDOWS

namespace Pine
{
	extern Application* CreateApplication();

	inline int RunApp(int argc, char** argv)
	{
		Log::Init();
		PE_CORE_WARN("Initialized Log!");

		auto app = Pine::CreateApplication();
		app->Run();
		delete app;

		return 0;
	}
}
#endif

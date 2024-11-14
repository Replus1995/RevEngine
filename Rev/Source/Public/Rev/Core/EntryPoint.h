#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Log.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Core/Application.h"
#include <type_traits>

#ifdef REV_PLATFORM_WINDOWS

namespace Rev
{
	template<class TApp, ENABLE_CHECK(TIsBaseOf<Application, TApp>)>
	int RunApp(int argc, char** argv)
	{
		Log::Init();
		REV_CORE_WARN("Initialized Log!");
		FFileSystem::Init(argv[0]);

		Application* app = new TApp();
		app->Run();
		delete app;

		FFileSystem::Shutdown();

		return 0;
	}
}
#endif

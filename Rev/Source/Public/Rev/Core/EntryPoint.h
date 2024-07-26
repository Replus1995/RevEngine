#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Log.h"
#include "Rev/Core/FileSystem.h"
#include "Rev/Core/Application.h"
#include <type_traits>

#ifdef RE_PLATFORM_WINDOWS

namespace Rev
{
	template<class TApp, class = typename std::enable_if<std::is_base_of_v<Application, TApp>>::type>
	int RunApp(int argc, char** argv)
	{
		Log::Init();
		RE_CORE_WARN("Initialized Log!");
		FFileSystem::Init()

		Application* app = new TApp();
		app->Run();
		delete app;

		return 0;
	}
}
#endif

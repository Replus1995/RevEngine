#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Core/Log.h"
#include "Rev/Core/Application.h"
#include "Rev/Misc/ArgParser.h"
#include <type_traits>

#ifdef REV_PLATFORM_WINDOWS

namespace Rev
{

template<class TApp, ENABLE_CHECK(std::is_base_of<Application, TApp>)>
int RunApp(int argc, char** argv)
{
	FLogger::Init();
	REV_CORE_WARN("Initialized Log!");
	GArgParser.ParseArgs(argc, argv);

	Application* app = new TApp();
	app->Run();
	delete app;

	return 0;
}

}
#endif

#pragma once

#ifdef PE_PLATFORM_WINDOWS

extern Pine::Application* Pine::CreateApplication();

int main(int argc, char** argv)
{
	Pine::Log::Init();
	PE_CORE_WARN("Initialized Log!");

	auto app = Pine::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif

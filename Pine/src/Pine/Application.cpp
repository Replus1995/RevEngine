#include "pinepch.h"
#include "Application.h"

#include "Pine/Events/ApplicationEvent.h"
#include "Pine/Log.h"

namespace Pine
{

	Application::Application()
	{
		mWindow = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (mRunning)
		{
			mWindow->OnUpdate();
		}
	}
}

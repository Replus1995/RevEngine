#include "Application.h"

#include "Pine/Events/ApplicationEvent.h"
#include "Pine/Log.h"

namespace Pine
{

	Application::Application()
	{
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		PINE_TRACE(e);

		while (true)
		{

		}
	}
}

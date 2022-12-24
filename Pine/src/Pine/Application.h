#pragma once

#include "Core.h"
#include "Window.h"

namespace Pine
{
	class PINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	private:
		std::unique_ptr<Window> mWindow;
		bool mRunning = true;
	};

	//To be defined in client
	Application* CreateApplication();
}



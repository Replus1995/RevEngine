#include "pinepch.h"
#include "Application.h"

#include "Pine/Log.h"

namespace Pine
{

	Application::Application()
	{
		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent, this));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (mRunning)
		{
			for (Layer* layer : mLayerStack)
				layer->OnUpdate();

			mWindow->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose, this));

		PINE_CORE_TRACE("{0}", e);

		for (auto iter = mLayerStack.end(); iter != mLayerStack.begin();)
		{
			(*--iter)->OnEvent(e);
			if (e.mHandled)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		mLayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		mLayerStack.PushOverlay(layer);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}
}

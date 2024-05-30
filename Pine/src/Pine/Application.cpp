#include "pinepch.h"
#include "Application.h"

#include "Pine/Log.h"

#include "glad/glad.h"

namespace Pine
{
	Application* Application::sInstance = nullptr;

	Application::Application()
	{
		PE_CORE_ASSERT(!sInstance, "Application already exists!");
		sInstance = this;

		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(PE_BIND_EVENT_FN(Application::OnEvent, this));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (mRunning)
		{
			glClearColor(.3f, .3f, .8f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			float deltaTime = mWindow->GetDeltaTime();
			for (Layer* layer : mLayerStack)
				layer->OnUpdate(deltaTime);

			mWindow->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(PE_BIND_EVENT_FN(Application::OnWindowClose, this));

		//PINE_CORE_TRACE("{0}", e);

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
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		mLayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}
}

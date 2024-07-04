#include "Rev/Core/Application.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Window.h"
#include "Rev/Core/Clock.h"
#include "Rev/Core/Input.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Renderer.h"
#include "Rev/Render/RenderCmd.h"

namespace Rev
{
	static ERenderAPI sRenderAPI = ERenderAPI::None;
	ERenderAPI GetRenderAPI()
	{
		return sRenderAPI;
	}

	Application* Application::sInstance = nullptr;
	Application::Application()
	{
		RE_CORE_ASSERT(!sInstance, "Application already exists!");
		sInstance = this;

		sRenderAPI = ERenderAPI::OpenGL;

		Input::InitState();

		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(RE_BIND_EVENT_FN(Application::OnEvent, this));

		Renderer::Init();
	}

	Application::~Application()
	{
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		Clock timer;

		while (mRunning)
		{
			float time = timer.Elapsed();
			float dt = time - mLastFrameTime;
			mLastFrameTime = time;

			//RenderCmd::SetClearColor(glm::vec4{ .3f, .3f, .8f, 1.0f });
			//RenderCmd::Clear();

			for (Layer* layer : mLayerStack)
				layer->OnUpdate(dt);

			mWindow->OnUpdate();
		}
	}

	void Application::Close()
	{
		mRunning = false;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(RE_BIND_EVENT_FN(Application::OnWindowClose, this));
		dispatcher.Dispatch<WindowResizeEvent>(RE_BIND_EVENT_FN(Application::OnWindowResize, this));
		//PINE_CORE_TRACE("{0}", e);

		Input::UpdateState(e);

		for (auto iter = mLayerStack.rbegin(); iter != mLayerStack.rend(); ++iter)
		{
			if (e.mHandled)
				break;
			(*iter)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		mLayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		mLayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			mMinimized = true;
			return false;
		}
		mMinimized = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}
}

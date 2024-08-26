#include "Rev/Core/Application.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Window.h"
#include "Rev/Core/Clock.h"
#include "Rev/Core/Input.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/RenderUtils.h"
#include "Rev/Asset/AssetLibrary.h"

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
		REV_CORE_ASSERT(!sInstance, "Application already exists!");
		sInstance = this;

		sRenderAPI = ERenderAPI::Vulkan;

		Input::InitState();

		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(RE_BIND_EVENT_FN(Application::OnEvent, this));

		RenderCmd::Init();
		RenderUtils::Init();
		FAssetLibrary::Init();
	}

	Application::~Application()
	{
		FAssetLibrary::Shutdown();
		RenderUtils::Shutdown();
		RenderCmd::Shutdown();
	}

	void Application::Run()
	{
		Clock Timer;

		while (mRunning)
		{
			int64 CurTime = Timer.ElapsedMillis();
			int64 DeltaTime = 0;
			bool bShouldUpdate = false;
			if (mLastFrameTime == 0)
			{
				mLastFrameTime = CurTime;
				bShouldUpdate = true;
			}
			else
			{
				DeltaTime = CurTime - mLastFrameTime;
				if (DeltaTime >= mFrameInterval)
				{
					mLastFrameTime = CurTime;
					bShouldUpdate = true;
				}
			}
			
			//RenderCmd::SetClearColor(glm::vec4{ .3f, .3f, .8f, 1.0f });
			//RenderCmd::Clear();
			if (bShouldUpdate)
			{
				float DeltaTimeSecond = DeltaTime * 0.001f * 0.001f;
				for (Layer* layer : mLayerStack)
					layer->OnUpdate(DeltaTimeSecond);
				RenderCmd::PresentFrame();
			}

			mWindow->OnUpdate();
		}
	}

	void Application::Close()
	{
		mRunning = false;
	}

	void Application::SetFpsLimit(uint32 InFps)
	{
		mFrameInterval = InFps > 0 ? 1000000 / InFps : 0;
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
		return false;
	}
}

#pragma once

#include "Rev/Core/Base.h"
#include "Rev/Core/LayerStack.h"
#include "Rev/Events/ApplicationEvent.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
	class Window;
	class Application
	{
	public:
		REV_API Application();
		REV_API virtual ~Application();

		REV_API void Run();
		REV_API void Close();
		REV_API void SetFpsLimit(uint32 InFps);

		REV_API void OnEvent(Event& e);
		REV_API void PushLayer(Layer* layer);
		REV_API void PushOverlay(Layer* overlay);

		REV_API inline bool Minimized() const { return mMinimized; }
		REV_API inline Window* GetWindow() { return mWindow.get(); }
		REV_API static Application& GetApp() { return *sInstance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> mWindow;
		bool mRunning = true;
		bool mMinimized = false;
		LayerStack mLayerStack;

		int64 mLastFrameTime = 0;
		int64 mFrameInterval = 0;

	private:
		static Application* sInstance;
	};

	//To be defined in client
	Application* CreateApplication();
}



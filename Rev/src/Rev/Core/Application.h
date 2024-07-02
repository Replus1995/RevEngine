#pragma once

#include "Rev/Core/Base.h"
#include "Rev/Core/LayerStack.h"
#include "Rev/Events/ApplicationEvent.h"
#include "Rev/Render/RenderCore.h"

namespace Rev
{
	class Window;
	class REV_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void Close();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline bool Minimized() const { return mMinimized; }
		inline Window* GetWindow() { return mWindow.get(); }
		static Application& GetApp() { return *sInstance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> mWindow;
		bool mRunning = true;
		bool mMinimized = false;
		LayerStack mLayerStack;
		float mLastFrameTime = 0.0f;

	private:
		static Application* sInstance;
	};

	//To be defined in client
	Application* CreateApplication();
}



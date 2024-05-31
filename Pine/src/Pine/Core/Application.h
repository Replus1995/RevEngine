#pragma once

#include "Pine/Core/Base.h"
#include "Pine/Core/LayerStack.h"
#include "Pine/Events/ApplicationEvent.h"
#include "Pine/Render/RenderCore.h"

namespace Pine
{
	class Window;
	class PINE_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void Close();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window* GetWindow() { return mWindow.get(); }

		static Application& GetApp() { return *sInstance; }
		static ERenderAPI GetRenderAPI() { return sRenderAPI; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

	private:
		std::unique_ptr<Window> mWindow;
		bool mRunning = true;
		bool mMinimized = false;
		LayerStack mLayerStack;

	private:
		static Application* sInstance;
		static ERenderAPI sRenderAPI;
	};

	//To be defined in client
	Application* CreateApplication();
}



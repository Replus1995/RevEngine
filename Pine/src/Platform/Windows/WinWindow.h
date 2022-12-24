#pragma once

#include "Pine/Window.h"

#include <GLFW/glfw3.h>

namespace Pine
{
	class WinWindow : public Window
	{
	public:
		WinWindow(const WindowProps& props);
		virtual ~WinWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return mData.Width; };
		inline unsigned int GetHeight() const override { return mData.Height; };

		inline void SetEventCallback(const EventCallbackFn& callback) override { mData.EventCallback = callback; };
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* mWindow;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
			EventCallbackFn EventCallback;
		};
		WindowData mData;
	};
}




#pragma once

#include "Pine/Core/Window.h"

#include <GLFW/glfw3.h>

namespace Pine
{
	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowProps& props);
		virtual ~GLFWWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return mData.Width; };
		inline unsigned int GetHeight() const override { return mData.Height; };

		inline void SetEventCallback(const EventCallbackFn& callback) override { mData.EventCallback = callback; };
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void SetClipboardText(const char* text) override;
		virtual const char* GetClipboardText() override;

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




#pragma once
#include "Rev/Core/Window.h"
#include <GLFW/glfw3.h>

namespace Rev
{
	class FGLFWWindow : public Window
	{
	public:
		FGLFWWindow(const WindowProps& props);
		virtual ~FGLFWWindow();

		void OnUpdate() override;

		inline virtual uint32 GetWidth() const override { return mData.Width; };
		inline virtual uint32 GetHeight() const override { return mData.Height; };
		virtual void GetFrameSize(int32& OutWidth, int32& OutHeight) const override;

		inline void SetEventCallback(const EventCallbackFn& callback) override { mData.EventCallback = callback; };
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void SetClipboardText(const char* text) override;
		virtual const char* GetClipboardText() override;

		virtual void* GetNativeHandle() const override { return mWindow; }

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




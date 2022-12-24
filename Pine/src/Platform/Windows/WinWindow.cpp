#include "pinepch.h"
#include "WinWindow.h"

#include "Pine/Log.h"

namespace Pine
{
	static bool sGLFWInitialized = false;

	Window* Window::Create(const WindowProps& props)
	{
		return new WinWindow(props);
	}


	WinWindow::WinWindow(const WindowProps& props)
	{
		Init(props);
	}

	WinWindow::~WinWindow()
	{
		Shutdown();
	}

	void WinWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(mWindow);
	}

	void WinWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		mData.VSync = enabled;
	}

	bool WinWindow::IsVSync() const
	{
		return mData.VSync;
	}

	void WinWindow::Init(const WindowProps& props)
	{
		mData.Title = props.Title;
		mData.Width = props.Width;
		mData.Height = props.Height;
		mData.VSync = true;

		PINE_CORE_INFO("Create Window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!sGLFWInitialized)
		{
			int success = glfwInit();
			PINE_CORE_ASSERT(success, "Could not initialze GLFW!");
			sGLFWInitialized = true;
		}

		mWindow = glfwCreateWindow((int)props.Width, (int)props.Height, mData.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(mWindow);
		glfwSetWindowUserPointer(mWindow, &mData);
		SetVSync(true);
	}

	void WinWindow::Shutdown()
	{
		glfwDestroyWindow(mWindow);
	}

}

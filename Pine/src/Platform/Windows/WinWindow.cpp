#include "pinepch.h"
#include "WinWindow.h"

#include "Pine/Log.h"
#include "Pine/Events/ApplicationEvent.h"
#include "Pine/Events/KeyEvent.h"
#include "Pine/Events/MouseEvent.h"

#include <glad/glad.h>

namespace Pine
{
	static bool sGLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		PE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

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

	float WinWindow::GetDeltaTime()
	{
		double time = glfwGetTime();
		float dt = mTime > 0.0 ? (float)(time - mTime) : (float)(1.0f / 60.0f);;
		mTime = time;
		return dt;
	}

	void WinWindow::SetClipboardText(const char* text)
	{
		glfwSetClipboardString(mWindow, text);
	}

	const char* WinWindow::GetClipboardText()
	{
		return glfwGetClipboardString(mWindow);
	}

	void WinWindow::Init(const WindowProps& props)
	{
		mData.Title = props.Title;
		mData.Width = props.Width;
		mData.Height = props.Height;
		mData.VSync = true;

		PE_CORE_INFO("Create Window {0} ({1}, {2})", props.Title, props.Width, props.Height);

		if (!sGLFWInitialized)
		{
			int success = glfwInit();
			PE_CORE_ASSERT(success, "Could not initialze GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			sGLFWInitialized = true;
		}

		mWindow = glfwCreateWindow((int)props.Width, (int)props.Height, mData.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(mWindow);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PE_CORE_ASSERT(status, "Failed to initialize Glad.");

		glfwSetWindowUserPointer(mWindow, &mData);
		SetVSync(true);

		//GLFW callbacks
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int width, int height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;
			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int key)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			KeyTypedEvent event(key);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WinWindow::Shutdown()
	{
		glfwDestroyWindow(mWindow);
	}

}

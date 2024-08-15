#include "GLFWWindow.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Assert.h"
#include "Rev/Events/ApplicationEvent.h"
#include "Rev/Events/KeyEvent.h"
#include "Rev/Events/MouseEvent.h"

namespace Rev
{
static bool sGLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description)
{
	REV_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
}

GLFWWindow::GLFWWindow(const WindowProps& props)
{
	mType = EWindowType::GLFW;
	Init(props);
}

GLFWWindow::~GLFWWindow()
{
	Shutdown();
}

void GLFWWindow::OnUpdate()
{
	glfwPollEvents();
	glfwSwapBuffers(mWindow);
}

void GLFWWindow::SetVSync(bool enabled)
{
	if (enabled)
		glfwSwapInterval(1);
	else
		glfwSwapInterval(0);

	mData.VSync = enabled;
}

bool GLFWWindow::IsVSync() const
{
	return mData.VSync;
}

void GLFWWindow::SetClipboardText(const char* text)
{
	glfwSetClipboardString(mWindow, text);
}

const char* GLFWWindow::GetClipboardText()
{
	return glfwGetClipboardString(mWindow);
}

void GLFWWindow::Init(const WindowProps& props)
{
	mData.Title = props.Title;
	mData.Width = props.Width;
	mData.Height = props.Height;
	mData.VSync = true;

	REV_CORE_INFO("Create Window {0} ({1}, {2})", props.Title, props.Width, props.Height);

	if (!sGLFWInitialized)
	{
		int success = glfwInit();
		REV_CORE_ASSERT(success, "Could not initialze GLFW!");

		switch (GetRenderAPI())
		{
		case ERenderAPI::Vulkan:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			break;
		}

		glfwSetErrorCallback(GLFWErrorCallback);
		sGLFWInitialized = true;
	}

	mWindow = glfwCreateWindow((int)props.Width, (int)props.Height, mData.Title.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(mWindow);

	//int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	//PE_CORE_ASSERT(status, "Failed to initialize Glad.");

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

void GLFWWindow::Shutdown()
{
	glfwDestroyWindow(mWindow);
}

}

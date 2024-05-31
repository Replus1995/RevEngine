#include "pinepch.h"
#include "Pine/Core/Window.h"
#include "Platform/GLFW/GLFWWindow.h"

namespace Pine
{

Window* Window::Create(const WindowProps& props)
{
	return new GLFWWindow(props);
}

}
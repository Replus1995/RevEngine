#include "pinepch.h"
#include "Pine/Core/Window.h"
#include "Platform/GLFW/GLFWWindow.h"

namespace Pine
{

Scope<Window> Window::Create(const WindowProps& props)
{
	return CreateScope<GLFWWindow>(props);
}

}
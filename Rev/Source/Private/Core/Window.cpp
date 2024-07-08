#include "Rev/Core/Window.h"
#include "Platform/GLFW/GLFWWindow.h"

namespace Rev
{

Scope<Window> Window::Create(const WindowProps& props)
{
	return CreateScope<GLFWWindow>(props);
}

}
#include "Rev/Core/Window.h"
#include "GLFW/GLFWWindow.h"

namespace Rev
{

Scope<Window> Window::Create(const WindowProps& props)
{
	return CreateScope<FGLFWWindow>(props);
}

}
#include "OpenGLRHI.h"
#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLShader.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLUniformBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLFrameBuffer.h"

namespace Rev
{

void OpenGLMessageCallback(
	unsigned source,
	unsigned type,
	unsigned id,
	unsigned severity,
	int length,
	const char* message,
	const void* userParam)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         RE_CORE_CRITICAL(message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:		 RE_CORE_ERROR(message); return;
	case GL_DEBUG_SEVERITY_LOW:          RE_CORE_WARN(message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: RE_CORE_TRACE(message); return;
	}

	RE_CORE_ASSERT(false, "[OpenGLRHI] Unknown severity level!");
}

void OpenGLRHI::Init()
{
	Window* pWnd = Application::GetApp().GetWindow();
	RE_CORE_ASSERT(pWnd, "[OpenGLRHI] Invalid window!");

	switch (pWnd->GetType())
	{
	case EWindowType::GLFW:
	{
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		RE_CORE_ASSERT(status, "[OpenGLRHI] Failed to initialize Glad.");
		break;
	}
	default:
		RE_CORE_ASSERT(false, "[OpenGLRHI] Unknown window type!");
	}

#ifdef RE_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLMessageCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
}

void OpenGLRHI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	glViewport(x, y, width, height);
}

void OpenGLRHI::SetClearColor(const Math::FLinearColor& color)
{
	glClearColor(color.R, color.G, color.B, color.A);
}

void OpenGLRHI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRHI::DrawIndexed(const VertexArray* vertexArray, uint32_t indexCount)
{
	vertexArray->Bind();
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

}
#include "pinepch.h"
#include "OpenGLRHI.h"
#include "Pine/Core/Base.h"
#include "Pine/Core/Assert.h"
#include "Pine/Core/Application.h"
#include "Pine/Core/Window.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "OpenGLShader.h"
#include "OpenGLVertexBuffer.h"
#include "OpenGLUniformBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLFrameBuffer.h"

namespace Pine
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
	case GL_DEBUG_SEVERITY_HIGH:         PE_CORE_CRITICAL(message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:		 PE_CORE_ERROR(message); return;
	case GL_DEBUG_SEVERITY_LOW:          PE_CORE_WARN(message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: PE_CORE_TRACE(message); return;
	}

	PE_CORE_ASSERT(false, "[OpenGLRHI] Unknown severity level!");
}

void OpenGLRHI::Init()
{
	Window* pWnd = Application::GetApp().GetWindow();
	PE_CORE_ASSERT(pWnd, "[OpenGLRHI] Invalid window!");

	switch (pWnd->GetType())
	{
	case EWindowType::GLFW:
	{
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		PE_CORE_ASSERT(status, "[OpenGLRHI] Failed to initialize Glad.");
		break;
	}
	default:
		PE_CORE_ASSERT(false, "[OpenGLRHI] Unknown window type!");
	}

#ifdef PE_DEBUG
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

void OpenGLRHI::SetClearColor(const glm::vec4& color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRHI::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRHI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount)
{
	vertexArray->Bind();
	uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

}
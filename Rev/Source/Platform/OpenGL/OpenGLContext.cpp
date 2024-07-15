#include "OpenGLContext.h"
#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include "Rev/Render/Material/Material.h"

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

void OpenGLContext::Init()
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

void OpenGLContext::SetViewport(uint32 x, uint32 y, uint32 width, uint32 height)
{
	glViewport(x, y, width, height);
}

void OpenGLContext::SetClearColor(const Math::FLinearColor& color)
{
	glClearColor(color.R, color.G, color.B, color.A);
}

void OpenGLContext::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLContext::PrepareMaterial(const Material* InMaterial)
{
	if(!InMaterial)
		return;
	switch (InMaterial->BlendMode)
	{
	case BM_Opaque:
		glDisable(GL_BLEND);
		break;
	case BM_Transparent:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case BM_Masked:
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	default:
		break;
	}

	if (InMaterial->TwoSided)
	{
		 glDisable(GL_CULL_FACE);
	}
	else
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
}

void OpenGLContext::DrawPrimitive(const MeshPrimitive* InPrimitive)
{
	if(!InPrimitive)
		return;
	GLenum DrawMode = TranslateDrawMode(InPrimitive->DrawMode);
	uint32 IndexCount = InPrimitive->VertexData->GetIndexBuffer()->GetCount();
	GLenum IndexType = TranslateIndexType(InPrimitive->VertexData->GetIndexBuffer()->GetStride());
	InPrimitive->VertexData->Bind();
	glDrawElements(DrawMode, IndexCount, IndexType, nullptr);
}

GLenum OpenGLContext::TranslateDrawMode(EMeshDrawMode InDrawMode)
{
	switch (InDrawMode)
	{
	case Rev::EMeshDrawMode::POINTS:
		return GL_POINTS;
	case Rev::EMeshDrawMode::LINE:
		return GL_LINE;
	case Rev::EMeshDrawMode::LINE_LOOP:
		return GL_LINE_LOOP;
	case Rev::EMeshDrawMode::LINE_STRIP:
		return GL_LINE_STRIP;
	case Rev::EMeshDrawMode::TRIANGLES:
		return GL_TRIANGLES;
	case Rev::EMeshDrawMode::TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	case Rev::EMeshDrawMode::TRIANGLE_FAN:
		return GL_TRIANGLE_FAN;
	default:
		break;
	}
	RE_CORE_ASSERT(false, "Unknown draw mode");
	return 0;
}

GLenum OpenGLContext::TranslateIndexType(uint32 InStride)
{
	switch (InStride)
	{
	case 1:
		return GL_UNSIGNED_BYTE;
	case 2:
		return GL_UNSIGNED_SHORT;
	case 4:
		return GL_UNSIGNED_INT;
	default:
		break;
	}
	RE_CORE_ASSERT(false, "Unknown index type");
	return 0;
}

}
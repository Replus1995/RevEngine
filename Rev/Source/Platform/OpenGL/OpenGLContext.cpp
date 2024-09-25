#include "OpenGLContext.h"
#include "Rev/Core/Base.h"
#include "Rev/Core/Assert.h"
#include "Rev/Core/Application.h"
#include "Rev/Core/Window.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Material/SurfaceMaterial.h"

#include "OpenGLShader.h"
#include "OpenGLBuffer.h"
#include "OpenGLTexture.h"
#include "OpenGLRenderTarget.h"

#include <GLFW/glfw3.h>

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
	case GL_DEBUG_SEVERITY_HIGH:         REV_CORE_CRITICAL(message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:		 REV_CORE_ERROR(message); return;
	case GL_DEBUG_SEVERITY_LOW:          REV_CORE_WARN(message); return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: REV_CORE_TRACE(message); return;
	}

	REV_CORE_ASSERT(false, "[OpenGLRHI] Unknown severity level!");
}

void FOpenGLContext::Init()
{
	Window* pWnd = Application::GetApp().GetWindow();
	REV_CORE_ASSERT(pWnd, "[OpenGLRHI] Invalid window!");

	switch (pWnd->GetType())
	{
	case EWindowType::GLFW:
	{
		int version = gladLoadGL(glfwGetProcAddress);
		REV_CORE_TRACE("[OpenGLRHI] OpenGL Version {0}.{1}", GLAD_VERSION_MAJOR(version), GLAD_VERSION_MINOR(version));
		break;
	}
	default:
		REV_CORE_ASSERT(false, "[OpenGLRHI] Unknown window type!");
	}

#ifdef REV_DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(OpenGLMessageCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
#endif

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}

void FOpenGLContext::Cleanup()
{
}

void FOpenGLContext::SetViewport(uint32 x, uint32 y, uint32 width, uint32 height)
{
	glViewport(x, y, width, height);
}

void FOpenGLContext::SetClearColor(const Math::FLinearColor& color)
{
	glClearColor(color.R, color.G, color.B, color.A);
}

void FOpenGLContext::ClearBackBuffer()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

//void FOpenGLContext::EnableDepthTest(bool bEnable)
//{
//	if(bEnable)
//		glEnable(GL_DEPTH_TEST);
//	else
//		glDisable(GL_DEPTH_TEST);
//}
//
//void FOpenGLContext::EnableDepthWrite(bool bEnable)
//{
//	glDepthMask(bEnable ? GL_TRUE : GL_FALSE);
//}
//
//void FOpenGLContext::SetDepthTestMode(EDepthTestMode InMode)
//{
//	GLenum DepthFunc = GL_LESS;
//	switch (InMode)
//	{
//	case Rev::DTM_Never:
//		DepthFunc = GL_NEVER;
//		break;
//	/*case Rev::DTM_Less:
//		DepthFunc = GL_LESS;
//		break;*/
//	case Rev::DTM_Equal:
//		DepthFunc = GL_EQUAL;
//		break;
//	case Rev::DTM_Greater:
//		DepthFunc = GL_GREATER;
//		break;
//	case Rev::DTM_NotEqual:
//		DepthFunc = GL_NOTEQUAL;
//		break;
//	case Rev::DTM_LessEqual:
//		DepthFunc = GL_LEQUAL;
//		break;
//	case Rev::DTM_GreaterEqual:
//		DepthFunc = GL_GEQUAL;
//		break;
//	case Rev::DTM_Always:
//		DepthFunc = GL_ALWAYS;
//		break;
//	default:
//		break;
//	}
//
//	glDepthFunc(DepthFunc);
//}
//
//void FOpenGLContext::SetBlendMode(EBlendMode InMode)
//{
//	switch (InMode)
//	{
//	case BM_Opaque:
//		glDisable(GL_BLEND);
//		break;
//	case BM_Transparent:
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		break;
//	case BM_Masked:
//		glEnable(GL_BLEND);
//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//		break;
//	default:
//		break;
//	}
//}
//
//void FOpenGLContext::SetCullFaceMode(ECullFaceMode InMode)
//{
//	switch (InMode)
//	{
//	case Rev::CFM_Disabled:
//		glDisable(GL_CULL_FACE);
//		break;
//	case Rev::CFM_Back:
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK);
//		break;
//	case Rev::CFM_Front:
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_FRONT);
//		break;
//	case Rev::CFM_BackAndFront:
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_FRONT_AND_BACK);
//		break;
//	default:
//		break;
//	}
//}

void FOpenGLContext::Bind(const Ref<FRHIVertexBuffer>& InVertexBuffer)
{
	GLuint VertexBufferName = InVertexBuffer ? *(const GLuint*)InVertexBuffer->GetNativeHandle() : 0;
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferName);
}

void FOpenGLContext::Bind(const Ref<FRHIIndexBuffer>& InIndexBuffer)
{
	GLuint IndexBufferName = InIndexBuffer ? *(const GLuint*)InIndexBuffer->GetNativeHandle() : 0;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferName);
}

void FOpenGLContext::Bind(const Ref<FRHIVertexArray>& InVertexArray)
{
	GLuint VertexArrayName = InVertexArray ? *(const GLuint*)InVertexArray->GetNativeHandle() : 0;
	glBindVertexArray(VertexArrayName);
}

void FOpenGLContext::Bind(const Ref<FRHIShaderProgram>& InProgram)
{
	GLuint ProgramName = InProgram ? *(const GLuint*)InProgram->GetNativeHandle() : 0;
	glUseProgram(ProgramName);
}

void FOpenGLContext::Bind(const Ref<FRHITexture>& InTexture, uint32 InUnit)
{
	GLuint TextureName = InTexture ? *(const GLuint*)InTexture->GetNativeHandle() : 0;
	GLuint SamplerName = InTexture ? *(const GLuint*)InTexture->GetSampler()->GetNativeHandle() : 0;
	glBindTextureUnit(InUnit, TextureName);
	glBindSampler(InUnit, SamplerName);
}

void FOpenGLContext::Bind(const Ref<FRHIRenderTarget>& InRenderTarget)
{
	GLuint RenderTargetName = InRenderTarget ? *(const GLuint*)InRenderTarget->GetNativeHandle() : 0;
	glBindFramebuffer(GL_FRAMEBUFFER, RenderTargetName);
}

void FOpenGLContext::Draw(const Ref<FRHIVertexArray>& InVertexArray)
{
	if(!InVertexArray)
		return;
	//GLenum DrawMode = TranslateDrawMode(InDrawMode);
	GLenum DrawMode = GL_TRIANGLES;
	uint32 IndexCount = InVertexArray->GetIndexBuffer()->GetCount();
	GLenum IndexType = TranslateIndexType(InVertexArray->GetIndexBuffer()->GetStride());
	Bind(InVertexArray);
	glDrawElements(DrawMode, IndexCount, IndexType, nullptr);
}

//GLenum FOpenGLContext::TranslateDrawMode(EDrawMode InDrawMode)
//{
//	switch (InDrawMode)
//	{
//	case Rev::DM_Points:
//		return GL_POINTS;
//	case Rev::DM_Line:
//		return GL_LINE;
//	case Rev::DM_LineLoop:
//		return GL_LINE_LOOP;
//	case Rev::DM_LineStrip:
//		return GL_LINE_STRIP;
//	case Rev::DM_Triangles:
//		return GL_TRIANGLES;
//	case Rev::DM_TriangleStrip:
//		return GL_TRIANGLE_STRIP;
//	case Rev::DM_TriangleFan:
//		return GL_TRIANGLE_FAN;
//	default:
//		break;
//	}
//	REV_CORE_ASSERT(false, "Unknown draw mode");
//	return 0;
//}

GLenum FOpenGLContext::TranslateIndexType(uint32 InStride)
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
	REV_CORE_ASSERT(false, "Unknown index type");
	return 0;
}

}
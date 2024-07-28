#include "OpenGLRenderTarget2D.h"

namespace Rev
{

static const uint32_t sMaxRenderTargetWH = 8192;

FOpenGLRenderTarget2D::FOpenGLRenderTarget2D(const FRenderTargetDesc& InDesc)
	: FRHIRenderTarget(InDesc)
{
	CreateResource();
}

FOpenGLRenderTarget2D::~FOpenGLRenderTarget2D()
{
	glDeleteFramebuffers(1, &mHandle);
}

void FOpenGLRenderTarget2D::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
	//glViewport(0, 0, mDesc.Width, mDesc.Height);
}

void FOpenGLRenderTarget2D::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FOpenGLRenderTarget2D::ClearTarget(ERenderTargetAttachment Index)
{
	if (Index == RTA_DepthStencilAttachment)
	{
		if(mDepthTexture)
			mDepthTexture->ClearData();
	}
	else if (Index >= 0 && Index < RTA_MaxColorAttachments)
	{
		auto& ColorTexture = mColorTextures[Index];
		if (ColorTexture)
			ColorTexture->ClearData();
	}
}

const Ref<FRHITexture> FOpenGLRenderTarget2D::GetTargetTexture(ERenderTargetAttachment Index) const
{
	if (Index == RTA_DepthStencilAttachment)
	{
		return mDepthTexture;
	}
	else if (Index >= 0 && Index < RTA_MaxColorAttachments)
	{
		return mColorTextures[Index];
	}
	return nullptr;
}

void FOpenGLRenderTarget2D::Recreate(uint32 InWidth, uint32 InHeight)
{
	if (InWidth == 0 || InHeight == 0 || InWidth > sMaxRenderTargetWH || InHeight > sMaxRenderTargetWH)
	{
		RE_CORE_WARN("Attempted to rezize framebuffer to {0}, {1} failed", InWidth, InHeight);
		return;
	}
	mDesc.Width = InWidth;
	mDesc.Height = InHeight;
	CreateResource();
}

void FOpenGLRenderTarget2D::CreateResource()
{
	bool bMultiSample = mDesc.NumSamples > 1;
	if (mHandle)
	{
		glDeleteFramebuffers(1, &mHandle);
		for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
		{
			mColorTextures[i].reset();
		}
		mDepthTexture.reset();
	}
	glCreateFramebuffers(1, &mHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);

	for (uint32 i = 0; i < mDesc.NumColorTargets; i++)
	{
		const FColorTargetDesc& SrcDesc = mDesc.ColorTargets[i];
		FTextureDesc DstDesc = FTextureDesc::MakeTexture2D(mDesc.Width, mDesc.Height, SrcDesc.Format, false, SrcDesc.ClearColor, 1, mDesc.NumSamples);
		Ref<FOpenGLTexture2D> ColorTexture = CreateRef<FOpenGLTexture2D>(DstDesc);

		GLuint TexHandle = *(GLuint*)ColorTexture->GetNativeHandle();
		GLenum TexTarget = bMultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, TexTarget, TexHandle, 0);

		mColorTextures[i] = std::move(ColorTexture);
	}

	if (mDesc.DepthStencilTarget.Format == PF_DepthStencil || mDesc.DepthStencilTarget.Format == PF_ShadowDepth)
	{
		const FDepthStencilTargetDesc& SrcDesc = mDesc.DepthStencilTarget;
		FTextureDesc DstDesc = FTextureDesc::MakeTexture2D(mDesc.Width, mDesc.Height, SrcDesc.Format, false, { SrcDesc.ClearDepth, SrcDesc.ClearStencil }, 1, mDesc.NumSamples);
		mDepthTexture = CreateRef<FOpenGLTexture2D>(DstDesc);

		GLuint TexHandle = *(GLuint*)mDepthTexture->GetNativeHandle();
		GLenum TexTarget = bMultiSample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, TexTarget, TexHandle, 0);
	}

	if (mDesc.NumColorTargets > 0)
	{
		std::vector<GLenum> ColorBuffers(mDesc.NumColorTargets);
		for (size_t i = 0; i < ColorBuffers.size(); i++)
		{
			ColorBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glDrawBuffers(ColorBuffers.size(), ColorBuffers.data());
	}
	else
	{
		//Depth Only
		glDrawBuffer(GL_NONE);
	}

	RE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "OpenGL RenderTarget2D Incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


}



#include "OpenGLRenderTarget.h"

namespace Rev
{

const uint16 FOpenGLRenderTarget::sMaxRenderTargetSize = 8192;

FOpenGLRenderTarget::FOpenGLRenderTarget(const FRenderTargetDesc& InDesc)
	: FRHIRenderTarget(InDesc)
{
	if (!IsEmptyTarget())
	{
		RE_CORE_ASSERT(InDesc.Width > 0 && InDesc.Width <= sMaxRenderTargetSize, "Invalid render target width");
		RE_CORE_ASSERT(InDesc.Height > 0 && InDesc.Height <= sMaxRenderTargetSize, "Invalid render target height");
	}
	CreateResource();
}

FOpenGLRenderTarget::~FOpenGLRenderTarget()
{
	ReleaseResource();
}

void FOpenGLRenderTarget::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, mHandle);
}

void FOpenGLRenderTarget::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FOpenGLRenderTarget::ResizeTargets(uint16 InWidth, uint16 InHeight)
{
	if(IsEmptyTarget())
		return;

	if (InWidth == 0 || InHeight == 0 || InWidth > sMaxRenderTargetSize || InHeight > sMaxRenderTargetSize)
	{
		RE_CORE_WARN("Attempted to rezize framebuffer to {0}, {1} failed", InWidth, InHeight);
		return;
	}
	if (mDesc.Width == InWidth && mDesc.Height == InHeight)
		return;

	mDesc.Width = InWidth;
	mDesc.Height = InHeight;
	ReleaseResource();
	CreateResource();
}

void FOpenGLRenderTarget::ClearTarget(ERenderTargetAttachment Index)
{
	if (Index == RTA_DepthStencilAttachment)
	{
		if (mDepthStencilAttachment.Texture)
		{
			if(mDepthStencilAttachment.ArrayIndex == -1)
				mDepthStencilAttachment.Texture->ClearAllData();
			else
				mDepthStencilAttachment.Texture->ClearLayerData(mDepthStencilAttachment.MipLevel, mDepthStencilAttachment.ArrayIndex);
		}
	}
	else if (Index >= 0 && Index < RTA_MaxColorAttachments)
	{
		if (mColorAttachments[Index].Texture)
		{
			if (mColorAttachments[Index].ArrayIndex == -1)
				mColorAttachments[Index].Texture->ClearAllData();
			else
				mColorAttachments[Index].Texture->ClearLayerData(mColorAttachments[Index].MipLevel, mColorAttachments[Index].ArrayIndex);
		}
			
	}
}

const Ref<FRHITexture> FOpenGLRenderTarget::GetTargetTexture(ERenderTargetAttachment Index) const
{
	if (Index == RTA_DepthStencilAttachment)
	{
		return mDepthStencilAttachment.Texture;
	}
	else if (Index >= 0 && Index < RTA_MaxColorAttachments)
	{
		return mColorAttachments[Index].Texture;
	}
	return nullptr;
}

void FOpenGLRenderTarget::Attach(ERenderTargetAttachment Index, const Ref<FRHITexture>& InTexture, uint8 InMipLevel, int32 InArrayIndex)
{
	if(!InTexture)
		return;
	FAttachment& TargetAttachment = Index < RTA_MaxColorAttachments ? mColorAttachments[Index] : mDepthStencilAttachment;
	GLenum AttachPoint = 0;
	if (Index < RTA_MaxColorAttachments)
		AttachPoint = GL_COLOR_ATTACHMENT0 + Index;
	else if (Index == RTA_DepthAttachment)
		AttachPoint = GL_DEPTH_ATTACHMENT;
	else if (Index == RTA_DepthStencilAttachment)
		AttachPoint = GL_DEPTH_STENCIL_ATTACHMENT;

	RE_CORE_ASSERT(AttachPoint != 0);

	GLuint TexHandle = *(GLuint*)InTexture->GetNativeHandle();
	if (InArrayIndex < 0)
	{
		glNamedFramebufferTexture(mHandle, AttachPoint, TexHandle, InMipLevel);
		TargetAttachment.ArrayIndex = -1;
	}
	else
	{
		RE_CORE_ASSERT(InArrayIndex < InTexture->GetArraySize(), "ArrayIndex out of range");
		glNamedFramebufferTextureLayer(mHandle, AttachPoint, TexHandle, InMipLevel, InArrayIndex);
		TargetAttachment.ArrayIndex = InArrayIndex;
	}
	TargetAttachment.MipLevel = InMipLevel;
	TargetAttachment.Texture = std::static_pointer_cast<FOpenGLTexture>(InTexture);
	mAttachmentsDirty = true;
}

void FOpenGLRenderTarget::Detach(ERenderTargetAttachment Index)
{
	FAttachment& TargetAttachment = Index < RTA_MaxColorAttachments ? mColorAttachments[Index] : mDepthStencilAttachment;
	GLenum AttachPoint = 0;
	if (Index < RTA_MaxColorAttachments)
		AttachPoint = GL_COLOR_ATTACHMENT0 + Index;
	else if (Index == RTA_DepthAttachment)
		AttachPoint = GL_DEPTH_ATTACHMENT;
	else if (Index == RTA_DepthStencilAttachment)
		AttachPoint = GL_DEPTH_STENCIL_ATTACHMENT;

	if (AttachPoint != 0)
	{
		glNamedFramebufferTexture(mHandle, AttachPoint, 0, 0);
	}
}

void FOpenGLRenderTarget::DetachAll()
{
	for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
	{
		Detach((ERenderTargetAttachment)i);
	}
	Detach(RTA_DepthStencilAttachment);
}

bool FOpenGLRenderTarget::IsEmptyTarget() const
{
	return mDesc.Dimension == ERenderTargetDimension::RenderTargetEmpty;
}

void FOpenGLRenderTarget::CreateResource()
{
	glCreateFramebuffers(1, &mHandle);
	if (!IsEmptyTarget())
	{
		for (uint32 i = 0; i < mDesc.NumColorTargets; i++)
		{
			if (auto ColorTexture = CreateColorTexture(mDesc.ColorTargets[i]); ColorTexture)
			{
				GLuint TexHandle = *(GLuint*)ColorTexture->GetNativeHandle();
				glNamedFramebufferTexture(mHandle, GL_COLOR_ATTACHMENT0 + i, TexHandle, 0);
				mColorAttachments[i].Texture = std::move(ColorTexture);
			}
		}
		if (auto DepthTexture = CreateDepthStencilTexture(mDesc.DepthStencilTarget); DepthTexture)
		{
			GLuint TexHandle = *(GLuint*)DepthTexture->GetNativeHandle();
			switch (DepthTexture->GetFormat())
			{
			case PF_ShadowDepth:
				glNamedFramebufferTexture(mHandle, GL_DEPTH_ATTACHMENT, TexHandle, 0);
				break;
			case PF_DepthStencil:
				glNamedFramebufferTexture(mHandle, GL_DEPTH_STENCIL_ATTACHMENT, TexHandle, 0);
				break;
			}
			mDepthStencilAttachment.Texture = std::move(DepthTexture);
		}
	}

	if (mDesc.NumColorTargets > 0)
	{
		std::vector<GLenum> ColorBuffers(mDesc.NumColorTargets);
		for (size_t i = 0; i < ColorBuffers.size(); i++)
		{
			ColorBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		glNamedFramebufferDrawBuffers(mHandle, ColorBuffers.size(), ColorBuffers.data());
	}
	else
	{
		//Depth Only
		glNamedFramebufferDrawBuffer(mHandle, GL_NONE);
	}
	RE_CORE_ASSERT(glCheckNamedFramebufferStatus(mHandle, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "OpenGL RenderTarget Incomplete!");
}

void FOpenGLRenderTarget::ReleaseResource()
{
	if (mHandle)
	{
		glDeleteFramebuffers(1, &mHandle);
		if (!IsEmptyTarget())
		{
			for (uint8 i = 0; i < RTA_MaxColorAttachments; i++)
			{
				//mColorAttachments[i].MipLevel = 0;
				//mColorAttachments[i].ArrayIndex = -1;
				mColorAttachments[i].Texture.reset();
			}
			//mDepthStencilAttachment.MipLevel = 0;
			//mDepthStencilAttachment.ArrayIndex = -1;
			mDepthStencilAttachment.Texture.reset();
		}
	}
}

Ref<FOpenGLTexture> FOpenGLRenderTarget::CreateColorTexture(const FColorTargetDesc& InDesc)
{
	RE_CORE_ASSERT(!IsEmptyTarget());
	if(InDesc.Format == PF_Unknown)
		return nullptr;
	FTextureDesc TexDesc ((ETextureDimension)mDesc.Dimension, InDesc.Format, false, mDesc.Width, mDesc.Height, 0, mDesc.ArraySize, InDesc.ClearColor, 1, mDesc.NumSamples);
	return CreateOpenGLTexture(TexDesc, FSamplerDesc());
}

Ref<FOpenGLTexture> FOpenGLRenderTarget::CreateDepthStencilTexture(const FDepthStencilTargetDesc& InDesc)
{
	RE_CORE_ASSERT(!IsEmptyTarget());
	if (InDesc.Format != PF_ShadowDepth && InDesc.Format != PF_DepthStencil)
		return nullptr;
	FTextureDesc TexDesc((ETextureDimension)mDesc.Dimension, InDesc.Format, false, mDesc.Width, mDesc.Height, 0, mDesc.ArraySize, { InDesc.ClearDepth, InDesc.ClearStencil }, 1, mDesc.NumSamples);
	return CreateOpenGLTexture(TexDesc, FSamplerDesc());
}


}



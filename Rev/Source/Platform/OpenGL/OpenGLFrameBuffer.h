#pragma once
#include <glad/glad.h>

namespace Rev
{

enum class FramebufferTextureFormat
{
	None = 0,
	// Color
	R32I,
	RGBA8,
	// Depth/stencil
	DEPTH24STENCIL8
};

struct FramebufferTextureDescription
{
	FramebufferTextureDescription() = default;
	FramebufferTextureDescription(FramebufferTextureFormat format)
		: TextureFormat(format) {}

	FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	// TODO: filtering/wrap
};

struct FramebufferAttachmentDescription
{
	FramebufferAttachmentDescription() = default;
	FramebufferAttachmentDescription(std::initializer_list<FramebufferTextureDescription> attachments)
		: Attachments(attachments) {}

	std::vector<FramebufferTextureDescription> Attachments;
};

struct FramebufferDescription
{
	uint32_t Width = 0, Height = 0;
	FramebufferAttachmentDescription Attachments;
	uint32_t Samples = 1;

	bool SwapChainTarget = false;
};

class OpenGLFramebuffer
{
public:
	OpenGLFramebuffer(const FramebufferDescription& desc);
	virtual ~OpenGLFramebuffer();

	void Invalidate();

	virtual void Bind();
	virtual void Unbind();

	virtual void Resize(uint32_t width, uint32_t height);
	virtual int ReadPixel(uint32_t attachmentIndex, int x, int y);

	virtual void ClearAttachment(uint32_t attachmentIndex, int value) ;

	virtual uint32_t GetAttachmentHandle(uint32_t index = 0) const;

	virtual const FramebufferDescription& GetDesc() const { return mDesc; }

public:
	static GLenum GetGLFormat(FramebufferTextureFormat format);

private:
	static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count);
	static void BindTexture(bool multisampled, uint32_t id);

	static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index);
	static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height);

private:
	uint32_t mHandle = 0;
	FramebufferDescription mDesc;

	std::vector<FramebufferTextureDescription> mColorAttachmentDescs;
	FramebufferTextureDescription mDepthAttachmentDesc = FramebufferTextureFormat::None;

	std::vector<uint32_t> mColorAttachmentHandles;
	uint32_t mDepthAttachmentHandle = 0;

};

}
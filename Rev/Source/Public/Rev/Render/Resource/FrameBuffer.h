#pragma once
#include "Rev/Core/Base.h"
#include <vector>

namespace Rev {

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

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

		virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
		virtual uint32_t GetAttachmentHandle(uint32_t attachmentIndex = 0) const = 0;

		virtual const FramebufferDescription& GetDesc() const = 0;

		static Ref<Framebuffer> Create(const FramebufferDescription& desc);
	};


}
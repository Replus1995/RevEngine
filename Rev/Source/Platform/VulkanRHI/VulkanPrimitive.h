#pragma once
#include "Rev/Render/RHI/RHIPrimitive.h"
#include <vulkan/vulkan.h>

namespace Rev
{

class FVulkanPrimitive : public FRHIPrimitive
{
public:
	FVulkanPrimitive(EPrimitiveTopology InTopology);
	virtual ~FVulkanPrimitive();

	virtual void AddVertexBuffer(const Ref<FRHIVertexBuffer>& VertexBuffer) override;
	virtual void SetIndexBuffer(const Ref<FRHIIndexBuffer>& IndexBuffer) override;

	virtual const std::vector<Ref<FRHIVertexBuffer>>& GetVertexBuffers() const override { return mVertexBuffers; };
	virtual const Ref<FRHIIndexBuffer>& GetIndexBuffer() const override { return mIndexBuffer; };
	virtual void Prepare() override;

	const std::vector<VkVertexInputBindingDescription>& GetBindingDescs() const { return mBindingDescs; }
	const std::vector<VkVertexInputAttributeDescription>& GetAttributeDescs() const { return mAttributeDescs; }
	uint64 GetDescHash() const { return mDescHash; }

protected:
	std::vector<Ref<FRHIVertexBuffer>> mVertexBuffers;
	Ref<FRHIIndexBuffer> mIndexBuffer;
	bool mInputDescDirty = false;
	std::vector<VkVertexInputBindingDescription> mBindingDescs;
	std::vector<VkVertexInputAttributeDescription> mAttributeDescs;
	uint64 mDescHash = 0; 
};

}
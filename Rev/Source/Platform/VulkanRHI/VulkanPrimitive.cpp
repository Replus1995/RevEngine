#include "VulkanPrimitive.h"
#include "VulkanBuffer.h"
#include "Core/VulkanEnum.h"

#include "Rev/Core/Hash.h"

namespace Rev
{

FVulkanPrimitive::FVulkanPrimitive(EPrimitiveTopology InTopology)
	: FRHIPrimitive(InTopology)
{

}

FVulkanPrimitive::~FVulkanPrimitive()
{

}

void FVulkanPrimitive::AddVertexBuffer(const Ref<FRHIVertexBuffer>& VertexBuffer)
{
	mVertexBuffers.push_back(VertexBuffer);
	mInputDescDirty = true;
}

void FVulkanPrimitive::SetIndexBuffer(const Ref<FRHIIndexBuffer>& IndexBuffer)
{
	mIndexBuffer = IndexBuffer;
}

void FVulkanPrimitive::PrepareDraw()
{
	if(!mInputDescDirty)
		return;
	mInputDescDirty = false;

	mBindingDescs.clear();
	mAttributeDescs.clear();

	for (size_t i = 0; i < mVertexBuffers.size(); i++)
	{
		auto VertexBuffer = static_cast<FVulkanVertexBuffer*>(mVertexBuffers[i].get());
		const auto& VertexLayout = VertexBuffer->GetLayout();

		uint32_t VertexStride = 0;
		for (const auto& Elem : VertexLayout)
		{
			VertexStride += Elem.GetElementSize();
			VkVertexInputAttributeDescription AttributeDesc{};
			AttributeDesc.binding = (uint32_t)i;
			AttributeDesc.location = Elem.Location;
			AttributeDesc.format = FVulkanEnum::Translate(Elem.Type);
			AttributeDesc.offset = Elem.Offset;
			mAttributeDescs.emplace_back(std::move(AttributeDesc));
		}

		VkVertexInputBindingDescription BindingDesc{};
		BindingDesc.binding = (uint32_t)i;
		BindingDesc.stride = VertexStride;
		BindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		mBindingDescs.emplace_back(std::move(BindingDesc));
	}

	mDescHash = uint64(FCityHash::Gen(mBindingDescs.data(), sizeof(VkVertexInputBindingDescription)) * mBindingDescs.size()) << 32
				& uint64(FCityHash::Gen(mAttributeDescs.data(), sizeof(VkVertexInputAttributeDescription) * mAttributeDescs.size()));
}

}


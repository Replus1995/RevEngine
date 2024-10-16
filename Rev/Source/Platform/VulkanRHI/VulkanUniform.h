#pragma once
#include "VulkanBuffer.h"
#include <set>

namespace Rev
{

class FVulkanUniformBuffer : public FRHIUniformBuffer, FVulkanBuffer
{
public:
	FVulkanUniformBuffer(uint32 InSize);
	virtual ~FVulkanUniformBuffer();
	virtual const void* GetNativeHandle() const override { return mBuffer; }
	virtual void UpdateSubData(const void* Data, uint32 Size, uint32 Offset) override;
};

}
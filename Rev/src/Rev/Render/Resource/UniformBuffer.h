#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class REV_API UniformBuffer
{
public:
	virtual ~UniformBuffer() {}
	virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;
	
	static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding);
};

}
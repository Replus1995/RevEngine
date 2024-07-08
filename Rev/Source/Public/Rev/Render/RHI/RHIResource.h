#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

class REV_API FRHIResource
{
protected:
	FRHIResource() = default;
public:
	virtual ~FRHIResource() = default;
	virtual void* GetNativeHandle() = 0;
};

}
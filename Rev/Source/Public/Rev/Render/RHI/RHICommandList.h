#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
class FRHIContext;
class FRHICommandList
{
public:
	FRHICommandList(FRHIContext* InContext);
	~FRHICommandList();

	FRHIContext* GetContext() { return mContext; };

protected:
	FRHIContext* mContext;
};

}
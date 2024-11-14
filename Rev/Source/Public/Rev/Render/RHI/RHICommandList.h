#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
class IRHIContext;
class FRHICommandList
{
public:
	FRHICommandList(IRHIContext* InContext);
	~FRHICommandList();

	IRHIContext* GetContext() { return mContext; };

protected:
	IRHIContext* mContext;
};

}
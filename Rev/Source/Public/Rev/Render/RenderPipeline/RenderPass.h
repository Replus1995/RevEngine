#pragma once
#include "Rev/Core/Base.h"
#include <string>

namespace Rev
{
class FRHIRenderTarget;
class FRHIRenderPass
{
public:
	FRHIRenderPass(std::string_view InName);
	virtual ~FRHIRenderPass();
	virtual void BeginPass() = 0;
	virtual void EndPass() = 0;

	const std::string& GetName() const { return mName; }
	const Ref<FRHIRenderTarget>& GetTarget() const { return mTarget; }

protected:
	std::string mName;
	Ref<FRHIRenderTarget> mTarget;
};

}
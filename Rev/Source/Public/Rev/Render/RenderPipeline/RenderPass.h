#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
class FRHIRenderTarget;
class FRenderPass
{
public:
	FRenderPass(std::string_view InName);
	virtual ~FRenderPass();
	virtual void BeginPass();
	virtual void EndPass();

	const std::string& GetName() const { return mName; }
	const Ref<FRHIRenderTarget>& GetTarget() const { return mTarget; }
	void SetTarget(const Ref<FRHIRenderTarget>& InTarget);

protected:
	std::string mName;
	Ref<FRHIRenderTarget> mTarget;
	bool mInPass = false;
};

}
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

	const Ref<FRHIRenderTarget>& GetRenderTarget() const;
	void SetRenderTarget(const Ref<FRHIRenderTarget>& InTarget);
	void ClearRenderTarget();

protected:
	std::string mName;
	Ref<FRHIRenderTarget> mRenderTarget;
	bool mInPass = false;
};

}
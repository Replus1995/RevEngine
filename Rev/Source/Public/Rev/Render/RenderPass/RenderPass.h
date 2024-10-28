#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{
class FRenderPass
{
public:
	FRenderPass(std::string_view InName);
	virtual ~FRenderPass();
	virtual void BeginPass();
	virtual void RunPass() = 0;
	virtual void EndPass();

	const std::string& GetName() const { return mName; }

protected:
	std::string mName;
	bool mInPass = false;
};

}
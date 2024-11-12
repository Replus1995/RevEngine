#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"

namespace Rev
{

class REV_API FRGPass
{
public:
	FRGPass(FRGName&& InName, ERGPassFlags InFlags);
	FRGPass(const FRGPass&) = delete;
	virtual ~FRGPass() = default;

	const char* GetName() const;

protected:
	const FRGName Name;
	const ERGPassFlags Flags;
	const ERHIPipeline Pipeline;
};

}
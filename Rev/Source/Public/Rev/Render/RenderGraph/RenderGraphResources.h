#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIResource.h"
#include "Rev/Render/RenderGraph/RenderGraphDefinitions.h"

namespace Rev
{

class REV_API FRGResource
{
public:
	FRGResource(const FRGResource&) = delete;
	virtual ~FRGResource() = default;

	// resource debug name
	const char* const Name = nullptr;

	FRHIResource* GetRHI() const { return ResourceRHI; }
protected:
	FRGResource(const char* InName) : Name(InName) {}
	FRHIResource* ResourceRHI = nullptr;

};

class REV_API FRGViewableResource : public FRGResource
{
public:
	virtual ~FRGViewableResource() = default;

	const ERGViewableResourceType Type;

	bool IsExternal() const { return bExternal; }
	bool IsExtracted() const { return bExtracted; }

protected:
	FRGViewableResource(const char* InName, ERGViewableResourceType InType);

	/** Whether this is an externally registered resource. */
	bool bExternal;

	/** Whether this is an extracted resource. */
	bool bExtracted;

	/** Whether any sub-resource has been used for write by a pass. */
	bool bProduced;

	/** Whether this resource is allocated through the transient resource allocator. */
	bool bTransient;
};

}

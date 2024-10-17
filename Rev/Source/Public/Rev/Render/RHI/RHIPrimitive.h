#pragma once
#include "Rev/Render/RHI/RHIDefinitions.h"
#include "Rev/Render/RHI/RHIBuffer.h"

namespace Rev
{

class FRHIPrimitive
{
public:
	FRHIPrimitive(EPrimitiveTopology InTopology) : mTopology(InTopology) {}
	virtual ~FRHIPrimitive() = default;

	virtual void AddVertexBuffer(const Ref<FRHIVertexBuffer>& VertexBuffer) = 0;
	virtual void SetIndexBuffer(const Ref<FRHIIndexBuffer>& IndexBuffer) = 0;

	virtual const std::vector<Ref<FRHIVertexBuffer>>& GetVertexBuffers() const = 0;
	virtual const Ref<FRHIIndexBuffer>& GetIndexBuffer() const = 0;

	EPrimitiveTopology GetTopology() const { return mTopology; }

protected:
	EPrimitiveTopology mTopology;
};

}
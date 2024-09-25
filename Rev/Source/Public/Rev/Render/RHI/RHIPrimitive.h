#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RHI/RHIBuffer.h"

namespace Rev
{

enum EPrimitiveTopology : uint8
{
	PT_Unknown = 0,
	PT_Points,
	PT_Lines,
	PT_LineStrip,
	PT_Triangles,
	PT_TriangleStrip,
	PT_TriangleFan
};

class FRHIPrimitive
{
public:
	FRHIPrimitive(EPrimitiveTopology InTopology) : mTopology(InTopology) {}
	virtual ~FRHIPrimitive() = default;

	virtual void AddVertexBuffer(const Ref<FRHIVertexBuffer>& VertexBuffer) = 0;
	virtual void SetIndexBuffer(const Ref<FRHIIndexBuffer>& IndexBuffer) = 0;

	virtual const std::vector<Ref<FRHIVertexBuffer>>& GetVertexBuffers() const = 0;
	virtual const Ref<FRHIIndexBuffer>& GetIndexBuffer() const = 0;
	virtual void Prepare() = 0;

protected:
	EPrimitiveTopology mTopology;
};

}
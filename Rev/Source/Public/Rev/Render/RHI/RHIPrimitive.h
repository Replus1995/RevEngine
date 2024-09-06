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
	FRHIPrimitive(EPrimitiveTopology InTopology);

	void AddVertexBuffer(const Ref<FRHIVertexBuffer>& InVertexBuffer);
	void SetIndexBuffer(const Ref<FRHIIndexBuffer>& InIndexBuffer);

	const std::vector<Ref<FRHIVertexBuffer>>& GetVertexBuffers() const { return mVertexBuffers; }
	const Ref<FRHIIndexBuffer>& GetIndexBuffer() const { return mIndexBuffer; }

private:
	EPrimitiveTopology mTopology;
	std::vector<Ref<FRHIVertexBuffer>> mVertexBuffers;
	Ref<FRHIIndexBuffer> mIndexBuffer;
};

}
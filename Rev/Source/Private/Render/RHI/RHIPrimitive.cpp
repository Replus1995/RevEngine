#include "Rev/Render/RHI/RHIPrimitive.h"

namespace Rev
{

FRHIPrimitive::FRHIPrimitive(EPrimitiveTopology InTopology)
	: mTopology(InTopology)
{
}

void FRHIPrimitive::AddVertexBuffer(const Ref<FRHIVertexBuffer>& InVertexBuffer)
{
	mVertexBuffers.push_back(InVertexBuffer);
}

void FRHIPrimitive::SetIndexBuffer(const Ref<FRHIIndexBuffer>& InIndexBuffer)
{
	mIndexBuffer = InIndexBuffer;
}

}



#pragma once
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Resource/RenderResource.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIState.h"
#include "Rev/Math/Maths.h"

namespace Rev
{

extern REV_API class FTexture* GWhiteTexture;
extern REV_API class FTexture* GBlackTexture;
extern REV_API class FTexture* GNormalTexture;

class FNullVertexBuffer : public FVertexBuffer
{
public:
	/**
	* Initialize the RHI for this rendering resource
	*/
	virtual void InitRHI() override
	{
		// create a static vertex buffer
		VertexBufferRHI = GDynamicRHI->RHICreateBuffer(sizeof(uint32) * 4, 0, EBufferUsageFlags::Static | EBufferUsageFlags::VertexBuffer);
		static const uint32 Vertices[4] = { 0, 0, 0, 0 };
		FRenderCore::GetMainContext()->RHIUpdateBufferData(VertexBufferRHI.get(), Vertices, sizeof(uint32) * 4);
	}
};
extern REV_API TGlobalResource<FNullVertexBuffer> GNullVertexBuffer;

class F2DQuadIndexBuffer : public FIndexBuffer
{
public:
	/**
	* Initialize the RHI for this rendering resource
	*/
	virtual void InitRHI() override
	{
		// create a static index buffer
		IndexBufferRHI = GDynamicRHI->RHICreateBuffer(sizeof(uint16) * 6, sizeof(uint16), EBufferUsageFlags::Static | EBufferUsageFlags::IndexBuffer);
		static const uint16 Indices[] = { 0, 1, 3, 0, 3, 2 };
		FRenderCore::GetMainContext()->RHIUpdateBufferData(IndexBufferRHI.get(), Indices, sizeof(uint16) * 6);
	}
};
extern REV_API TGlobalResource<F2DQuadIndexBuffer> G2DQuadIndexBuffer;

class F2DQuadVertexBuffer : public FVertexBuffer
{
public:
	/**
	* Initialize the RHI for this rendering resource
	*/
	virtual void InitRHI() override
	{
		// create a static vertex buffer
		VertexBufferRHI = GDynamicRHI->RHICreateBuffer(sizeof(Math::FVector2) * 4, sizeof(Math::FVector2), EBufferUsageFlags::Static | EBufferUsageFlags::VertexBuffer);
		static const Math::FVector2 Vertices[4] =
		{
			Math::FVector2(-1,-1),
			Math::FVector2(-1,+1),
			Math::FVector2(+1,-1),
			Math::FVector2(+1,+1),
		};
		FRenderCore::GetMainContext()->RHIUpdateBufferData(VertexBufferRHI.get(), Vertices, sizeof(Math::FVector2) * 4);
	}
};
extern REV_API TGlobalResource<F2DQuadVertexBuffer> G2DQuadVertexBuffer;

class FTileVertexInputState : public FVertexInputState
{
public:

	/** Destructor. */
	virtual ~FTileVertexInputState() {}

	virtual void InitRHI()
	{
		FRHIVertexInputStateDesc StateDesc;
		StateDesc.Add(FRHIVertexElement(EVertexElmentType::Float2, 0, 0, sizeof(Math::FVector2)));
		VertexInputStateRHI = GDynamicRHI->RHICreateVertexInputState(StateDesc);
	}

	virtual void ReleaseRHI()
	{
		VertexInputStateRHI.reset();
	}
};
extern REV_API TGlobalResource<FTileVertexInputState> GTileVertexInputState;



class FStaticMeshVertexInputState : public FVertexInputState
{
public:
	/** Destructor. */
	virtual ~FStaticMeshVertexInputState() {}

	virtual void InitRHI()
	{
		FRHIVertexInputStateDesc StateDesc;
		StateDesc.Add(FRHIVertexElement(EVertexElmentType::Float3, 0, 0, sizeof(Math::FVector3))); //Position
		StateDesc.Add(FRHIVertexElement(EVertexElmentType::Color, 1, 1, sizeof(Math::FColor))); //Color
		StateDesc.Add(FRHIVertexElement(EVertexElmentType::Float3, 2, 2, sizeof(Math::FVector3))); //Normal
		StateDesc.Add(FRHIVertexElement(EVertexElmentType::Float4, 3, 3, sizeof(Math::FVector4))); //Tangent
		StateDesc.Add(FRHIVertexElement(EVertexElmentType::Float2, 4, 4, sizeof(Math::FVector2))); //TexCoord0
		StateDesc.Add(FRHIVertexElement(EVertexElmentType::Float2, 5, 5, sizeof(Math::FVector2))); //TexCoord1
		VertexInputStateRHI = GDynamicRHI->RHICreateVertexInputState(StateDesc);
	}
};
extern REV_API TGlobalResource<FStaticMeshVertexInputState> GStaticMeshVertexInputState;

class FDefaultSamplerState : public FRenderResource
{
public:
	Ref<FRHISamplerState> SamplerStateRHI;

	FDefaultSamplerState() {}
	virtual ~FDefaultSamplerState() {}
	virtual void InitRHI()
	{
		FRHISamplerStateDesc StateDesc;
		SamplerStateRHI = GDynamicRHI->RHICreateSamplerState(StateDesc);
	}
	virtual void ReleaseRHI() override
	{
		SamplerStateRHI.reset();
	}
};
extern REV_API TGlobalResource<FDefaultSamplerState> GDefaultSamplerState;

class FRHICommandList;
class REV_API FRenderUtils
{
public:
	static void Init();
	static void Shutdown();
	static void PostProcessDraw(FRHICommandList& RHICmdList);

};

}
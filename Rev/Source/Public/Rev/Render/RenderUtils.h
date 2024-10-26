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
		FRenderCore::GetMainContext()->UpdateBufferData(IndexBufferRHI.get(), Indices, sizeof(uint16) * 6);
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
		FRenderCore::GetMainContext()->UpdateBufferData(VertexBufferRHI.get(), Vertices, sizeof(Math::FVector2) * 4);
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
		StateDesc.Add(FVertexElement(EVertexElmentType::Float2, 0, 0, sizeof(Math::FVector2)));
		VertexInputStateRHI = GDynamicRHI->RHICreateVertexInputState(StateDesc);
	}

	virtual void ReleaseRHI()
	{
		VertexInputStateRHI.reset();
	}
};
extern REV_API TGlobalResource<FTileVertexInputState> GTileVertexInputState;

class FNullVertexBuffer: public FVertexBuffer
{
public:
	/**
	* Initialize the RHI for this rendering resource
	*/
	virtual void InitRHI() override
	{
		// create a static vertex buffer
		VertexBufferRHI = GDynamicRHI->RHICreateBuffer(sizeof(float) * 4, 0, EBufferUsageFlags::Static | EBufferUsageFlags::VertexBuffer);
		static const float Vertices[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		FRenderCore::GetMainContext()->UpdateBufferData(VertexBufferRHI.get(), Vertices, sizeof(float) * 4);
	}
};
extern REV_API TGlobalResource<FNullVertexBuffer> GNullVertexBuffer;


class FStaticMeshVertexInputState : public FVertexInputState
{
public:
	/** Destructor. */
	virtual ~FStaticMeshVertexInputState() {}

	virtual void InitRHI()
	{
		FRHIVertexInputStateDesc StateDesc;
		StateDesc.Add(FVertexElement(EVertexElmentType::Float3, 0, 0, sizeof(Math::FVector3))); //Position
		StateDesc.Add(FVertexElement(EVertexElmentType::Color, 1, 1, sizeof(Math::FColor))); //Color
		StateDesc.Add(FVertexElement(EVertexElmentType::Float3, 2, 2, sizeof(Math::FVector3))); //Normal
		StateDesc.Add(FVertexElement(EVertexElmentType::Float4, 3, 3, sizeof(Math::FVector4))); //Tangent
		StateDesc.Add(FVertexElement(EVertexElmentType::Float2, 4, 4, sizeof(Math::FVector2))); //TexCoord0
		StateDesc.Add(FVertexElement(EVertexElmentType::Float2, 5, 5, sizeof(Math::FVector2))); //TexCoord1
		VertexInputStateRHI = GDynamicRHI->RHICreateVertexInputState(StateDesc);
	}
};
extern REV_API TGlobalResource<FStaticMeshVertexInputState> GStaticMeshVertexInputState;

class REV_API RenderUtils
{
public:
	static void Init();
	static void Shutdown();
};

}
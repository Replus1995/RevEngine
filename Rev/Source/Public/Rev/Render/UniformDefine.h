#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Math/Maths.h"
#include "Rev/Utils/Rect2D.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/RenderCmd.h"
#include "Rev/Render/UniformLayout.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/RHICore.h"

namespace Rev
{

template<typename T, uint32 Binding>
struct TUniform
{
public:
	T Data;
	Ref<FRHIUniformBuffer> Resource;

	TUniform() = default;
	TUniform(const TUniform<T, Binding>&) = delete;

	void AllocResource() { Resource = FRHICore::CreateUniformBuffer(sizeof(T), Binding); }
	void FreeResource() { Resource.reset(); }

	void Upload(uint32 InSize = 0, uint32 InOffset = 0) const
	{
		Upload(&Data, InSize, InOffset);
	}

	void Upload(const void* InData, uint32 InSize = 0, uint32 InOffset = 0) const
	{
		uint32 UploadSize = InSize > 0 ? InSize : sizeof(T);
		const void* UploadData = InData ? InData : &Data;
		Resource->UpdateSubData(UploadData, UploadSize, InOffset);
	}

};

struct alignas(16) FSceneUniform
{
	FRect2D ViewExtent;
	Math::FVector4 ViewPos;
	Math::FMatrix4 ViewMat;
	Math::FMatrix4 ProjMat;
	Math::FMatrix4 ViewProjMat;
	Math::FMatrix4 InvViewProjMat;
};

struct alignas(16) FStaticMeshUniform
{
	Math::FMatrix4 ModelMat;
};

struct FShadowUniform
{
	Math::FMatrix4 LightProjMats[6];
	Math::FMatrix4 LightViewMats[6];
	Math::FVector4 LightParams; //X = ViewCount
};

struct FDirectionalLightUniform
{
	uint32 Count = 0;
	struct alignas(16) Info {
		Math::FVector4 Direction;
		Math::FVector4 Color;
		float Intensity;
		int32 ShadowIndex;
		uint32 ShadowCount;
	}  Lights[REV_MAX_DIRECTIONAL_LIGHTS];
};

struct FSpotLightUniform
{
	uint32 Count = 0;
	struct alignas(16) Info {
		Math::FVector4 Position;
		Math::FVector4 Direction;
		Math::FVector4 Color;
		float Intensity;
		int32 ShadowIndex;
		float Radius;
		float Angle;
	} Lights[REV_MAX_SPOT_LIGHTS];
};

struct FPointLightUniform
{
	uint32 Count = 0;
	struct alignas(16) Info {
		Math::FVector4 Position;
		Math::FVector4 Color;
		float Intensity;
		int32 ShadowIndex;
		float Radius;
		float Padding;
	} Lights[REV_MAX_POINT_LIGHTS];
};

}
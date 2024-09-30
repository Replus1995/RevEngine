#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

enum EPixelFormat : uint16
{
	PF_Unknown				= 0,

	PF_R8					= 1, //UNORM
	PF_R8_UINT				= 2,
	PF_R8_SINT				= 3,
	PF_R16					= 4, //UNORM
	PF_R16_UINT				= 5,
	PF_R16_SINT				= 6,
	PF_R16_FLOAT			= 7,
	PF_R32_UINT				= 8,
	PF_R32_SINT				= 9,
	PF_R32_FLOAT			= 10,

	PF_R8G8					= 11,
	PF_R16G16				= 12,
	PF_R16G16_FLOAT			= 13,
	PF_R32G32_FLOAT			= 14,

	PF_R8G8B8				= 15,
	PF_R16G16B16			= 16,
	PF_R16G16B16_FLOAT		= 17,
	PF_R32G32B32_UINT		= 18,
	PF_R32G32B32_SINT		= 19,
	PF_R32G32B32_FLOAT		= 20,

	PF_R8G8B8A8				= 21,
	PF_R8G8B8A8_SNORM		= 22,
	PF_R8G8B8A8_UINT		= 23,
	PF_R8G8B8A8_SINT		= 24,
	PF_R16G16B16A16			= 25,
	PF_R16G16B16A16_SNORM	= 26,
	PF_R16G16B16A16_UINT	= 27,
	PF_R16G16B16A16_SINT	= 28,
	PF_R16G16B16A16_FLOAT	= 29,
	PF_R32G32B32A32_UINT	= 30,
	PF_R32G32B32A32_SINT	= 31,
	PF_R32G32B32A32_FLOAT	= 32,

	PF_DepthStencil			= 33,
	PF_ShadowDepth			= 34,

	PF_Count
};

struct FPixelFormatInfo
{
	FPixelFormatInfo() = delete;
	FPixelFormatInfo(
		EPixelFormat InFormat,
		const char* InName,
		uint8 InBlockSizeX,
		uint8 InBlockSizeY,
		uint8 InBlockSizeZ,
		uint8 InBlockBytes,
		uint8 InNumComponents,
		bool InSupported
	);


	const char* Name;
	EPixelFormat Format;
	uint8 BlockSizeX;
	uint8 BlockSizeY;
	uint8 BlockSizeZ;
	uint8 BlockBytes;
	uint8 NumComponents;
	bool Supported;

	uint32 PlatformFormat;


	// Utils
	static bool HasDepth(EPixelFormat InFormat);
	static bool HasStencil(EPixelFormat InFormat);
};

extern REV_API FPixelFormatInfo GPixelFormats[PF_Count];

}
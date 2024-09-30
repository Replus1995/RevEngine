#include "Rev/Render/PixelFormat.h"

namespace Rev
{

bool FPixelFormatInfo::HasDepth(EPixelFormat InFormat)
{
	return InFormat == PF_DepthStencil || InFormat == PF_ShadowDepth;
}

bool FPixelFormatInfo::HasStencil(EPixelFormat InFormat)
{
	return InFormat == PF_DepthStencil;
}


FPixelFormatInfo::FPixelFormatInfo(EPixelFormat InFormat, const char* InName, uint8 InBlockSizeX, uint8 InBlockSizeY, uint8 InBlockSizeZ, uint8 InBlockBytes, uint8 InNumComponents, bool InSupported)
	: Name(InName)
	, Format(InFormat)
	, BlockSizeX(InBlockSizeX)
	, BlockSizeY(InBlockSizeY)
	, BlockSizeZ(InBlockSizeZ)
	, BlockBytes(InBlockBytes)
	, NumComponents(InNumComponents)
	, Supported(InSupported)
	, PlatformFormat(0)
{
}

FPixelFormatInfo GPixelFormats[PF_Count] = {
	//				 Format					Name			BlockSizeX	BlockSizeY	BlockSizeZ	BlockBytes	NumComponents	Supported
	FPixelFormatInfo(PF_Unknown,			"Unknown",				0,			0,			0,			0,			0,			0),
	FPixelFormatInfo(PF_R8,					"R8",					1,			1,			1,			1,			1,			1),
	FPixelFormatInfo(PF_R8_UINT,			"R8_UINT",				1,			1,			1,			1,			1,			1),
	FPixelFormatInfo(PF_R8_SINT,			"R8_SINT",				1,			1,			1,			1,			1,			1),
	FPixelFormatInfo(PF_R16,				"R16",					1,			1,			1,			2,			1,			1),
	FPixelFormatInfo(PF_R16_UINT,			"R16_UINT",				1,			1,			1,			2,			1,			1),
	FPixelFormatInfo(PF_R16_SINT,			"R16_SINT",				1,			1,			1,			2,			1,			1),
	FPixelFormatInfo(PF_R16_FLOAT,			"R16_FLOAT",			1,			1,			1,			2,			1,			1),
	FPixelFormatInfo(PF_R32_UINT,			"R32_UINT",				1,			1,			1,			4,			1,			1),
	FPixelFormatInfo(PF_R32_SINT,			"R32_SINT",				1,			1,			1,			4,			1,			1),
	FPixelFormatInfo(PF_R32_FLOAT,			"R32_FLOAT",			1,			1,			1,			4,			1,			1),
	FPixelFormatInfo(PF_R8G8,				"R8G8",					1,			1,			1,			2,			2,			1),
	FPixelFormatInfo(PF_R16G16,				"R16G16",				1,			1,			1,			4,			2,			1),
	FPixelFormatInfo(PF_R16G16_FLOAT,		"R16G16_FLOAT",			1,			1,			1,			4,			2,			1),
	FPixelFormatInfo(PF_R32G32_FLOAT,		"R32G32_FLOAT",			1,			1,			1,			8,			2,			1),
	FPixelFormatInfo(PF_R8G8B8,				"R8G8B8",				1,			1,			1,			3,			3,			1),
	FPixelFormatInfo(PF_R16G16B16,			"R16G16B16",			1,			1,			1,			6,			3,			1),
	FPixelFormatInfo(PF_R16G16B16_FLOAT,	"R16G16B16_FLOAT",		1,			1,			1,			6,			3,			1),
	FPixelFormatInfo(PF_R32G32B32_UINT,		"R32G32B32_UINT",		1,			1,			1,			12,			3,			1),
	FPixelFormatInfo(PF_R32G32B32_SINT,		"R23G32B32_SINT",		1,			1,			1,			12,			3,			1),
	FPixelFormatInfo(PF_R32G32B32_FLOAT,	"R32G32B32_FLOAT",		1,			1,			1,			12,			3,			1),
	FPixelFormatInfo(PF_R8G8B8A8,			"R8G8B8A8",				1,			1,			1,			4,			4,			1),
	FPixelFormatInfo(PF_R8G8B8A8_SNORM,		"R8G8B8A8_SNORM",		1,			1,			1,			4,			4,			1),
	FPixelFormatInfo(PF_R8G8B8A8_UINT,		"R8G8B8A8_UINT",		1,			1,			1,			4,			4,			1),
	FPixelFormatInfo(PF_R8G8B8A8_SINT,		"R8G8B8A8_SINT",		1,			1,			1,			4,			4,			1),
	FPixelFormatInfo(PF_R16G16B16A16,		"R16G16B16A16",			1,			1,			1,			8,			4,			1),
	FPixelFormatInfo(PF_R16G16B16A16_SNORM,	"R16G16B16A16_SNORM",	1,			1,			1,			8,			4,			1),
	FPixelFormatInfo(PF_R16G16B16A16_UINT,	"R16G16B16A16_UINT",	1,			1,			1,			8,			4,			1),
	FPixelFormatInfo(PF_R16G16B16A16_SINT,	"R16G16B16A16_SINT",	1,			1,			1,			8,			4,			1),
	FPixelFormatInfo(PF_R16G16B16A16_FLOAT,	"R16G16B16A16_FLOAT",	1,			1,			1,			8,			4,			1),
	FPixelFormatInfo(PF_R32G32B32A32_UINT,	"R32G32B32A32_UINT",	1,			1,			1,			16,			4,			1),
	FPixelFormatInfo(PF_R32G32B32A32_SINT,	"R32G32B32A32_SINT",	1,			1,			1,			16,			4,			1),
	FPixelFormatInfo(PF_R32G32B32A32_FLOAT,	"R32G32B32A32_FLOAT",	1,			1,			1,			16,			4,			1),

	FPixelFormatInfo(PF_DepthStencil,		"DepthStencil",			1,			1,			1,			4,			1,			0),
	FPixelFormatInfo(PF_ShadowDepth,		"ShadowDepth",			1,			1,			1,			4,			1,			0),
};




}



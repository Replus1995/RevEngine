#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

//Sampler
enum ESamplerFilterMode : uint8
{
	SF_Nearest = 0,
	SF_Bilinear,
	SF_Trilinear,
	SF_AnisotropicNearest,
	SF_AnisotropicLinear,
};

enum ESamplerWarpMode : uint8
{
	SW_Repeat = 0,
	SW_Clamp,
	SW_Mirror,
	SW_Border
};

enum ESamplerCompareFunction : uint8
{
	SCF_Never,
	SCF_Less
};

//Texture
enum class ETextureDimension : uint8
{
	Texture2D = 1,
	Texture2DArray = 2,
	TextureCube = 3,
	TextureCubeArray = 4,
	Texture3D = 5,
};

enum class ETextureCreateFlags : uint64
{
	None							= 0,
	RenderTargetable				= 1ull << 0,
	ResolveTargetable				= 1ull << 1,
	DepthStencilTargetable			= 1ull << 2,
	ShaderResource					= 1ull << 3,
	SRGB							= 1ull << 4,
};
ENUM_CLASS_FLAGS(ETextureCreateFlags)

enum ETextureCubeFace : uint8
{
	TCF_PositiveX = 0,
	TCF_NegativeX = 1,
	TCF_PositiveY = 2,
	TCF_NegativeY = 3,
	TCF_PositiveZ = 4,
	TCF_NegativeZ = 5,
	TCF_Count = 6
};

//RenderTarget
enum ERenderTargetAttachment : uint8
{
	RTA_ColorAttachment0 = 0,
	RTA_ColorAttachment1 = 1,
	RTA_ColorAttachment2 = 2,
	RTA_ColorAttachment3 = 3,
	RTA_ColorAttachment4 = 4,
	RTA_ColorAttachment5 = 5,
	RTA_ColorAttachment6 = 6,
	RTA_ColorAttachment7 = 7,
	RTA_MaxColorAttachments = 8,
	RTA_DepthAttachment = 9,
	RTA_DepthStencilAttachment = 10,
	RTA_EmptyAttachment = 16
};

//Shader
enum class ERHIUniformType : uint8
{
	Sampler,
	Image,
	Buffer,
};

enum class ERHIShaderStage : uint8
{
	Unknown = 0,
	Vertex = 1,
	Pixel = 2,
	Hull = 3, //unsupported
	Domain = 4, //unsupported
	Geometry = 5, //unsupported
	Compute = 6, //unsupported
	Count = 7
};



//Primitive
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

//Pipeline
enum EFillMode : uint8
{
	PM_Fill = 0,
	PM_Line = 1,
	PM_Point = 2
};

enum ECullMode : uint8
{
	CM_None = 0,
	CM_Back = 1,
	CM_Front = 2,
	CM_BackAndFront = 3
};

enum EDepthClipMode : uint8
{
	DCM_Clip,
	DCM_Clamp
};

enum EStencilOperation
{
	SO_Keep = 0,
	SO_Zero = 1,
	SO_Replace = 2,
	SO_IncrementAndClamp = 3,
	SO_DecrementAndClamp = 4,
	SO_Invert = 5,
	SO_IncrementAndWrap = 6,
	SO_DecrementAndWrap = 7,
};

enum ECompareFunction : uint8
{
	CF_Never = 0,
	CF_Less = 1,
	CF_Equal = 2,
	CF_Greater = 3,
	CF_NotEqual = 4,
	CF_LessEqual = 5,
	CF_GreaterEqual = 6,
	CF_Always = 7
};

enum EBlendFactor : uint8
{
	BF_Zero = 0,
	BF_One = 1,
	BF_SrcColor = 2,
	BF_OneMinusSrcColor = 3,
	BF_DstColor = 4,
	BF_OneMinusDstColor = 5,
	BF_SrcAlpha = 6,
	BF_OneMinusSrcAlpha = 7,
	BF_DstAlpha = 8,
	BF_OneMinusDstAlpha = 9,
	BF_ConstantColor = 10,
	BF_OneMinusConstantColor = 11,
	BF_ConstantAlpha = 12,
	BF_OneMinusConstantAlpha = 13,
};

enum EBlendOperation : uint8
{
	BO_Add = 0,
	BO_Subtract = 1,
	BO_ReverseSubtract = 2,
	BO_Min = 3,
	BO_Max = 4,
};

enum EColorWriteMask : uint8
{
	CWM_Red = 0x00000001,
	CWM_Green = 0x00000002,
	CWM_Blue = 0x00000004,
	CWM_Alpha = 0x00000008,
	CWM_RGB = CWM_Red | CWM_Green | CWM_Blue,
	CWM_RGBA = CWM_RGB | CWM_Alpha
};

enum EPipelineBindPoint : uint8
{
	PBP_Graphics,
	PBP_Compute,
	PBP_RayTracing
};

//RenderPass

enum EAttachmentLoadOp : uint8
{
	ALO_Load = 0,
	ALO_Clear,
	ALO_DontCare
};

enum EAttachmentStoreOp : uint8
{
	ASO_Store = 0,
	ASO_DontCare
};

}
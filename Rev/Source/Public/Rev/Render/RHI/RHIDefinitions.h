#pragma once
#include "Rev/Core/Base.h"

#define REV_MAX_VERTEX_ELEMENTS 16

namespace Rev
{
//Resource
enum EResourceLockMode
{
	RLM_ReadOnly,
	RLM_WriteOnly,
};

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
	ColorTarget				= 1ull << 0,
	ColorResolveTarget				= 1ull << 1,
	DepthStencilTarget			= 1ull << 2,
	DepthStencilResolveTarget	= 1ull << 3,
	ShaderResource					= 1ull << 4,
	SRGB							= 1ull << 5,
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
enum class EShaderUniformType : uint8
{
	Unknown,
	Buffer,
	Sampler,
	Texture,
};

enum EShaderStage : uint8
{
	SS_Vertex = 0,
	SS_Pixel = 1,
	SS_Geometry = 2,
	SS_Hull = 3, //has not been supported
	SS_Domain = 4, //has not been supported
	SS_Compute = 5,
	SS_Unknown = 6,
	SS_Count = 7,
	SS_NumGraphics = 5,
	SS_NumCompute = 1
};

//Buffer
enum class EBufferUsageFlags : uint32
{
	None				= 0,
	Static				= 1u << 0,
	Dynamic				= 1u << 1,
	Volatile			= 1u << 2,
	ShaderResource		= 1u << 3,
	KeepCPUAccess		= 1u << 4,
	VertexBuffer		= 1u << 5,
	IndexBuffer			= 1u << 6,
	StorageBuffer		= 1u << 7,
};
ENUM_CLASS_FLAGS(EBufferUsageFlags);


//Vertex
enum class EVertexElmentType : uint8
{
	Unknown	= 0,
	Float1	= 1,
	Float2	= 2,
	Float3	= 3,
	Float4	= 4,
	UByte4	= 5,
	Color	= 6,
	Matrix3	= 7,
	Matrix4	= 8,
	UInt	= 9,
	Bool	= 10
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
	CF_Never		= 0,
	CF_Less			= 1,
	CF_Equal		= 2,
	CF_LessEqual	= 3,
	CF_Greater		= 4,
	CF_NotEqual		= 5,
	CF_GreaterEqual = 6,
	CF_Always		= 7
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

//Utils
enum class ERHIPipeline : uint8
{
	None		= 0,
	Graphics	= 1 << 0,
	Compute		= 1 << 1,
	RayTracing	= 1 << 2,
	All			= Graphics | Compute | RayTracing,
	Num			= 3
};
ENUM_CLASS_FLAGS(ERHIPipeline)

struct FRHIDeviceCapacity
{
	uint8 MaxNumSamples = 1;
};

}
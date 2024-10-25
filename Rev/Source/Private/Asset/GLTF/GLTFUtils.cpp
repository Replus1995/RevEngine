#include "GLTFUtils.h"
#include "Rev/Core/Assert.h"
#include "Rev/Render/Component/StaticMesh.h"
#include <filesystem>

#include "GLTFDebug.h"

#define TINYGLTF_USE_CPP14
#define TINYGLTF_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <tiny_gltf.h>

namespace Rev
{

namespace
{

FBuffer UnpackTightBuffer(const uint8* InDataMem, const tinygltf::BufferView& InBufferView, size_t DstStride)
{
	FBuffer OutBuffer(InBufferView.byteLength);
	size_t SrcOffset = 0;
	size_t DstOffset = 0;
	while (SrcOffset < InBufferView.byteLength)
	{
		const uint8* SrcMem = InDataMem + SrcOffset;
		uint8* DstMem = OutBuffer.Data() + DstOffset;

		memcpy(DstMem, SrcMem, DstStride);

		SrcOffset += InBufferView.byteStride;
		DstOffset += DstStride;
	}
	return OutBuffer;
}

FBuffer LoadBufferData(const tinygltf::Accessor& InAccessor, const tinygltf::Model& InModel, size_t DstStride)
{
	REV_CORE_ASSERT(InAccessor.bufferView >= 0 && InAccessor.bufferView < InModel.bufferViews.size());
	const tinygltf::BufferView& RefBufferView = InModel.bufferViews[InAccessor.bufferView];
	const tinygltf::Buffer& RefBuffer = InModel.buffers[RefBufferView.buffer];
	const uint8* DataMem = RefBuffer.data.data() + RefBufferView.byteOffset + InAccessor.byteOffset;

	if (RefBufferView.byteStride == 0)
	{
		FBuffer OutBuffer(RefBufferView.byteLength);
		memcpy(OutBuffer.Data(), DataMem, RefBufferView.byteLength);
		return OutBuffer;
	}
	else
	{
		//Tightly Packed
		return UnpackTightBuffer(DataMem, RefBufferView, DstStride);
	}
}

template<typename T, size_t N>
FBuffer LoadBufferData(const tinygltf::Accessor& InAccessor, const tinygltf::Model& InModel)
{
	constexpr size_t DstStride = sizeof(T) * N;
	return LoadBufferData(InAccessor, InModel, DstStride);
}

FBuffer LoadTexCoordData(const tinygltf::Accessor& InAccessor, const tinygltf::Model& InModel)
{
	REV_CORE_ASSERT(InAccessor.bufferView >= 0 && InAccessor.bufferView < InModel.bufferViews.size());
	const tinygltf::BufferView& BufferView = InModel.bufferViews[InAccessor.bufferView];
	const tinygltf::Buffer& Buffer = InModel.buffers[BufferView.buffer];
	const uint8* DataMem = Buffer.data.data() + BufferView.byteOffset + InAccessor.byteOffset;
	const uint32 Count = BufferView.byteLength / BufferView.byteStride;

	FBuffer OutBuffer;
	OutBuffer.Allocate<Math::FVector2>(Count);
	for (uint32 i = 0; i < Count; i++)
	{
		Math::FVector2& UV = OutBuffer.DataAs<Math::FVector2>()[i];
		switch (InAccessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
		{
			float* Src = (float*)(DataMem + BufferView.byteStride * i);
			UV.X = Src[0];
			UV.Y = Src[1];
 		}
		break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		{
			uint8* Src = (uint8*)(DataMem + BufferView.byteStride * i);
			UV.X = float(Src[0]) / 255.0f;
			UV.Y = float(Src[1]) / 255.0f;
		}
		break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		{
			uint16* Src = (uint16*)(DataMem + BufferView.byteStride * i);
			UV.X = float(Src[0]) / 65535.0f;
			UV.Y = float(Src[1]) / 65535.0f;
		}
		break;
		default:
			break;
		}
	}
	return OutBuffer;
}

FBuffer LoadColorData(const tinygltf::Accessor& InAccessor, const tinygltf::Model& InModel)
{
	REV_CORE_ASSERT(InAccessor.bufferView >= 0 && InAccessor.bufferView < InModel.bufferViews.size());
	const tinygltf::BufferView& BufferView = InModel.bufferViews[InAccessor.bufferView];
	const tinygltf::Buffer& Buffer = InModel.buffers[BufferView.buffer];
	const uint8* DataMem = Buffer.data.data() + BufferView.byteOffset + InAccessor.byteOffset;
	const uint32 Count = BufferView.byteLength / BufferView.byteStride;

	FBuffer OutBuffer;
	OutBuffer.Allocate<Math::FColor>(Count);
	for (uint32 i = 0; i < Count; i++)
	{
		Math::FLinearColor LinearColor;
		switch (InAccessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
		{
			float* Src = (float*)(DataMem + BufferView.byteStride * i);
			LinearColor.R = Src[0];
			LinearColor.G = Src[1];
			LinearColor.B = Src[2];
			if (InAccessor.type == TINYGLTF_TYPE_VEC4)
				LinearColor.A = Src[3];
		}
		break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		{
			uint8* Src = (uint8*)(DataMem + BufferView.byteStride * i);
			LinearColor.R = float(Src[0]) / 255.0f;
			LinearColor.G = float(Src[1]) / 255.0f;
			LinearColor.B = float(Src[2]) / 255.0f;
			if (InAccessor.type == TINYGLTF_TYPE_VEC4)
				LinearColor.A = float(Src[3]) / 255.0f;
		}
		break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		{
			uint16* Src = (uint16*)(DataMem + BufferView.byteStride * i);
			LinearColor.R = float(Src[0]) / 65535.0f;
			LinearColor.G = float(Src[1]) / 65535.0f;
			LinearColor.B = float(Src[2]) / 65535.0f;
			if (InAccessor.type == TINYGLTF_TYPE_VEC4)
				LinearColor.A = float(Src[3]) / 65535.0f;
		}
		break;
		default:
			break;
		}

		Math::FColor& Color = OutBuffer.DataAs<Math::FColor>()[i];
		Color = Math::FLinearColor::ToSRGB(LinearColor);
	}
	return OutBuffer;
}

template<typename IndexType>
FBuffer LoadIndexData(const tinygltf::Accessor& InAccessor, const tinygltf::Model& InModel)
{
	REV_CORE_ASSERT(InAccessor.bufferView >= 0 && InAccessor.bufferView < InModel.bufferViews.size());
	const tinygltf::BufferView& BufferView = InModel.bufferViews[InAccessor.bufferView];
	const tinygltf::Buffer& Buffer = InModel.buffers[BufferView.buffer];
	const uint8* DataMem = Buffer.data.data() + BufferView.byteOffset + InAccessor.byteOffset;
	const uint32 Count = BufferView.byteLength / BufferView.byteStride;

	FBuffer OutBuffer;
	OutBuffer.Allocate<IndexType>(Count);
	for (uint32 i = 0; i < Count; i++)
	{
		IndexType& IndexValue = OutBuffer.DataAs<IndexType>()[i];
		switch (InAccessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_BYTE:
			IndexValue = *(int8*)(DataMem + BufferView.byteStride * i);
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			IndexValue = *(uint8*)(DataMem + BufferView.byteStride * i);
			break;
		case TINYGLTF_COMPONENT_TYPE_SHORT:
			IndexValue = *(int16*)(DataMem + BufferView.byteStride * i);
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			IndexValue = *(uint16*)(DataMem + BufferView.byteStride * i);
			break;
		case TINYGLTF_COMPONENT_TYPE_INT:
			IndexValue = *(int32*)(DataMem + BufferView.byteStride * i);
			break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			IndexValue = *(uint32*)(DataMem + BufferView.byteStride * i);
			break;
		default:
			break;
		}
	}
	return OutBuffer;
}

}


uint32 FGLTFUtils::GetIndexStride(int InComponentType)
{
	switch (InComponentType)
	{
	case TINYGLTF_COMPONENT_TYPE_BYTE:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
	case TINYGLTF_COMPONENT_TYPE_SHORT:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		return sizeof(uint16);
	case TINYGLTF_COMPONENT_TYPE_INT:
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		return sizeof(uint32);
	default:
		return 0;
	}
}

EPrimitiveTopology FGLTFUtils::TranslatePrimitiveTopology(int InPrimitiveTopology)
{
	switch (InPrimitiveTopology)
	{
	case TINYGLTF_MODE_POINTS:
		return PT_Points;
	case TINYGLTF_MODE_LINE:
		return PT_Lines;
	case TINYGLTF_MODE_LINE_LOOP:
		return PT_Lines;
	case TINYGLTF_MODE_LINE_STRIP:
		return PT_LineStrip;
	case TINYGLTF_MODE_TRIANGLES:
		return PT_Triangles;
	case TINYGLTF_MODE_TRIANGLE_STRIP:
		return PT_TriangleStrip;
	case TINYGLTF_MODE_TRIANGLE_FAN:
		return PT_TriangleFan;
	default:
		return PT_Triangles;
	}
}

EPixelFormat FGLTFUtils::TranslateImageFormat(const tinygltf::Image& InImage)
{
	switch (InImage.component)
	{
	case 1:
	{
		switch (InImage.pixel_type)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			return PF_R8;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			return PF_R16;
		default:
			break;
		}
		break;
	}
	case 2:
	{
		break;
	}
	case 3:
	{
		switch (InImage.pixel_type)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			return PF_R8G8B8;
		default:
			break;
		}
		break;
	}
	case 4:
	{
		switch (InImage.pixel_type)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			return PF_R8G8B8A8;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			return PF_R32G32B32A32_FLOAT;
		default:
			break;
		}
		break;
	}
	default:
		break;
	}

	REV_CORE_WARN("[GLTF] Unsupported image format.");
	return PF_Unknown;
}

FRHISamplerStateDesc FGLTFUtils::TranslateSampler(const tinygltf::Sampler& InSampler)
{
	FRHISamplerStateDesc Result;

	switch (InSampler.minFilter)
	{
	case TINYGLTF_TEXTURE_FILTER_NEAREST:
	case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
	case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
		Result.Filter = SF_Nearest;
		break;
	case TINYGLTF_TEXTURE_FILTER_LINEAR:
	case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
		Result.Filter = SF_Bilinear;
		break;
	case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
		Result.Filter = SF_Trilinear;
		break;
	default:
		break;
	}

	switch (InSampler.wrapS)
	{
	case TINYGLTF_TEXTURE_WRAP_REPEAT:
		Result.WarpU = SW_Repeat;
		break;
	case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
		Result.WarpU = SW_Clamp;
		break;
	case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
		Result.WarpU = SW_Mirror;
		break;
	default:
		break;
	}

	switch (InSampler.wrapT)
	{
	case TINYGLTF_TEXTURE_WRAP_REPEAT:
		Result.WarpV = SW_Repeat;
		break;
	case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
		Result.WarpV = SW_Clamp;
		break;
	case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
		Result.WarpV = SW_Mirror;
		break;
	default:
		break;
	}

	return Result;
}

Math::FLinearColor FGLTFUtils::TranslateColor(const std::vector<double>& InColor)
{
	REV_CORE_ASSERT(InColor.size() == 4);
	Math::FLinearColor Result(0, 0, 0, 1);
	for (size_t i = 0; i < InColor.size(); i++)
	{
		Result[i] = InColor[i];
 	}
	return Result;
}

Math::FVector3 FGLTFUtils::TranslateVector3(const std::vector<double>& InVector3)
{
	REV_CORE_ASSERT(InVector3.size() == 3);
	Math::FVector3 Result(0, 0, 0);
	for (size_t i = 0; i < InVector3.size(); i++)
	{
		Result[i] = InVector3[i];
	}
	return Result;
}

Ref<FStaticMesh> FGLTFUtils::ImportMeshPrimitive(const tinygltf::Primitive& InPrimitive, const tinygltf::Model& InModel, const std::vector<Ref<FMaterial>>& InMaterials)
{
	uint32 MaterialIndex = InPrimitive.material;
	bool bGenNormals = true;
	bool bGenTangents = true;
	FStaticMeshBuilder MeshBuilder;

	struct {
		uint32 NumVertices = 0;
		uint32 NumTexCoord = 0;
		FBuffer PositionData;
		FBuffer NormalData;
		FBuffer TangentData;
		FBuffer TexCoordData[2];
		FBuffer ColorData;

		uint32 NumIndices = 0;
		bool b32Bit = false;
		FBuffer IndexData;

	} Storage;

	for (auto& [attrName, attrIndex] : InPrimitive.attributes)
	{
		REV_CORE_ASSERT(attrIndex >= 0 && attrIndex < InModel.accessors.size());
		const tinygltf::Accessor& RefAccessor = InModel.accessors[attrIndex];
		if (attrName.compare("POSITION") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC3);
			Storage.PositionData = LoadBufferData<float, 3>(RefAccessor, InModel);
			Storage.NumVertices = Storage.PositionData.Size() / sizeof(Math::FVector3);
		}
		else if (attrName.compare("NORMAL") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC3);
			Storage.NormalData = LoadBufferData<float, 3>(RefAccessor, InModel);
			bGenNormals = false;
		}
		else if (attrName.compare("TANGENT") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC4);
			Storage.TangentData = LoadBufferData<float, 3>(RefAccessor, InModel);
			bGenTangents = false;
		}
		else if (attrName.compare("TEXCOORD_0") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC2);
			Storage.TexCoordData[0] = LoadTexCoordData(RefAccessor, InModel);
			Storage.NumTexCoord++;
		}
		else if (attrName.compare("TEXCOORD_1") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC2);
			Storage.TexCoordData[1] = LoadTexCoordData(RefAccessor, InModel);
			Storage.NumTexCoord++;
		}
		else if (attrName.compare("COLOR_0") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC3 || RefAccessor.type == TINYGLTF_TYPE_VEC4);
			Storage.ColorData = LoadColorData(RefAccessor, InModel);
		}
	}

	MeshBuilder.InitVertices(Storage.NumVertices, Storage.NumVertices, 
		std::move(Storage.PositionData), 
		std::move(Storage.NormalData),
		std::move(Storage.TangentData),
		Storage.TexCoordData,
		std::move(Storage.ColorData));

	{
		//Load Index Data
		REV_CORE_ASSERT(InPrimitive.indices >= 0 && InPrimitive.indices < InModel.accessors.size());
		const tinygltf::Accessor&  RefAccessor = InModel.accessors[InPrimitive.indices];
		REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_SCALAR);

		uint32 IndexStride = GetIndexStride(RefAccessor.componentType);
		Storage.b32Bit = IndexStride == 4;
		if (Storage.b32Bit)
		{
			Storage.IndexData = LoadIndexData<uint32>(RefAccessor, InModel);
		}
		else
		{
			Storage.IndexData = LoadIndexData<uint16>(RefAccessor, InModel);
		}
		Storage.NumIndices = Storage.IndexData.Size() / IndexStride;
		MeshBuilder.InitIndices(Storage.NumIndices, Storage.b32Bit, std::move(Storage.IndexData));
	}
	MeshBuilder.SetMaterials({ InMaterials[MaterialIndex] });

	return MeshBuilder.Build(bGenNormals, bGenTangents);
}

std::vector<Ref<FStaticMesh>> FGLTFUtils::ImportStaticMesh(const tinygltf::Mesh& InMesh, const tinygltf::Model& InModel, const std::vector<Ref<FMaterial>>& InMaterials)
{
	std::map<int, int> MaterialIndexMap;
	std::vector<Ref<FStaticMesh>> OutMeshes;
	OutMeshes.reserve(InMesh.primitives.size());
	//OutStorage->Name = InMesh.name;
	for (size_t i = 0; i < InMesh.primitives.size(); i++) {
		Ref<FStaticMesh> StaticMesh = ImportMeshPrimitive(InMesh.primitives[i], InModel, InMaterials);
		OutMeshes.push_back(StaticMesh);
	}
	return OutMeshes;
}

Ref<FTextureStorage> FGLTFUtils::ImportTexture(const tinygltf::Texture& InTexture, const tinygltf::Model& InModel)
{
	const tinygltf::Image& InImage = InModel.images[InTexture.source];
	const tinygltf::Sampler& InSampler = InModel.samplers[InTexture.sampler];

	Ref<FTextureStorage> Storage = CreateRef<FTextureStorage>();
	Storage->TextureDesc = FRHITextureDesc::Make2D(InImage.width, InImage.height, TranslateImageFormat(InImage));
	Storage->SamplerDesc = TranslateSampler(InSampler);
	{
		Storage->ImageData.Resize(1, 1);
		FBuffer& ImageBuffer = Storage->ImageData.At(0, 0);
		//Decoded data
		ImageBuffer.Allocate(InImage.image.size());
		memcpy(ImageBuffer.Data(), InImage.image.data(), InImage.image.size());
	}
	//TODO: Decode image data from buffer
	
	return Storage;
}

Ref<FMaterial> FGLTFUtils::ImportMaterial(const tinygltf::Material& InMaterial, const tinygltf::Model& InModel, const std::vector<Ref<FTextureStorage>>& InTextures)
{
	auto& pbrInfo = InMaterial.pbrMetallicRoughness;
	FPBRMaterialStorage Storage;
	//Storage.Name = InMaterial.name;
	
	{
		//Alpha Mode
		if (InMaterial.alphaMode == "OPAQUE")
		{
			Storage.BlendMode = MBM_Opaque;
		}
		else if (InMaterial.alphaMode == "BLEND")
		{
			Storage.BlendMode = MBM_Transparent;
		}
		else if (InMaterial.alphaMode == "MASK")
		{
			Storage.BlendMode = MBM_Masked;
		}
	}

	Storage.MaskClip = InMaterial.alphaCutoff;
	Storage.TwoSided = InMaterial.doubleSided;
	Storage.BaseColorFactor = TranslateColor(pbrInfo.baseColorFactor);
	Storage.BaseColorTexture = pbrInfo.baseColorTexture.index >= 0 ? InTextures[pbrInfo.baseColorTexture.index]->CreateTexture(true) : nullptr;
	Storage.Metallic = pbrInfo.metallicFactor;
	Storage.Roughness = pbrInfo.roughnessFactor;
	Storage.MetallicRoughnessTexture = pbrInfo.metallicRoughnessTexture.index >= 0 ? InTextures[pbrInfo.metallicRoughnessTexture.index]->CreateTexture(false) : nullptr;
	Storage.NormalScale = InMaterial.normalTexture.scale;
	Storage.NormalTexture = InMaterial.normalTexture.index >= 0 ? InTextures[InMaterial.normalTexture.index]->CreateTexture(false) : nullptr;
	Storage.OcclusionStrength = InMaterial.occlusionTexture.strength;
	Storage.OcclusionTexture = InMaterial.occlusionTexture.index >= 0 ? InTextures[InMaterial.occlusionTexture.index]->CreateTexture(false) : nullptr;
	Storage.EmissiveFactor = TranslateVector3(InMaterial.emissiveFactor);
	Storage.EmissiveTexture = InMaterial.emissiveTexture.index >= 0 ? InTextures[InMaterial.emissiveTexture.index]->CreateTexture(true) : nullptr;

	return Storage.CreateMaterial();
}

FModelImportResult FGLTFUtils::ImportModel(const FPath& InPath, bool DumpInfo)
{
	tinygltf::Model InModel;
	tinygltf::TinyGLTF ctx;
	std::string err;
	std::string warn;
	
	std::string NativePath = InPath.ToNative();
	bool ret = false;
	if (InPath.Extension().compare(".glb") == 0) {
	
	    REV_CORE_INFO("Reading binary glTF {0}", NativePath);
	    // assume binary glTF.
	    ret = ctx.LoadBinaryFromFile(&InModel, &err, &warn, NativePath.c_str());
	}
	else {
	    REV_CORE_INFO("Reading ASCII glTF {0}", NativePath);
	    // assume ascii glTF.
	    ret = ctx.LoadASCIIFromFile(&InModel, &err, &warn, NativePath.c_str());
	}
	
	if (!warn.empty()) {
	    REV_CORE_WARN("glTF load warning: {}", warn);
	}
	
	if (!err.empty()) {
	    REV_CORE_ERROR("glTF load error: {}", err);
	}
	
	if (!ret) {
	    REV_CORE_ERROR("glTF load failed: {}", NativePath);
		return {};
	}
	
	if (DumpInfo)
	{
	    FGLTFDebug::DumpModelInfo(InModel);
	}

	FModelImportResult Result;
	std::vector<Ref<FTextureStorage>> TextureStorageVec;

	for (size_t i = 0; i < InModel.textures.size(); i++)
	{
		Ref<FTextureStorage> TextureStorage = ImportTexture(InModel.textures[i], InModel);
		TextureStorageVec.emplace_back(std::move(TextureStorage));
	}

	for (size_t i = 0; i < InModel.materials.size(); i++)
	{
		Ref<FMaterial> Material = ImportMaterial(InModel.materials[i], InModel, TextureStorageVec);
		Result.Materials.emplace_back(std::move(Material));
	}

	for (size_t i = 0; i < InModel.meshes.size(); i++)
	{
		std::vector<Ref<FStaticMesh>> StaticMeshes = ImportStaticMesh(InModel.meshes[i], InModel, Result.Materials);
		Result.StaticMeshes = std::move(StaticMeshes);

		//TODO: Import skeleton mesh
	}

	return Result;
}

}
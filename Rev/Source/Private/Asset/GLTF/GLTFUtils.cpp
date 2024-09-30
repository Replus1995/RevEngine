#include "GLTFUtils.h"
#include "Rev/Core/Assert.h"
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

}


uint32 FGLTFUtils::GetComponentSize(int InComponentType)
{
	switch (InComponentType)
	{
	case TINYGLTF_COMPONENT_TYPE_BYTE:
		return sizeof(int8);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
		return sizeof(uint8);
	case TINYGLTF_COMPONENT_TYPE_SHORT:
		return sizeof(int16);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		return sizeof(uint16);
	case TINYGLTF_COMPONENT_TYPE_INT:
		return sizeof(int32);
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
		return sizeof(uint32);
	case TINYGLTF_COMPONENT_TYPE_FLOAT:
		return sizeof(float);
	case TINYGLTF_COMPONENT_TYPE_DOUBLE:
		return sizeof(double);
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
			return PF_RGB8;
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
			return PF_RGBA8;
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			return PF_RGBA32F;
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

FSamplerDesc FGLTFUtils::TranslateSampler(const tinygltf::Sampler& InSampler)
{
	FSamplerDesc Result;

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

Ref<FMeshPrimitiveStorage> FGLTFUtils::ImportMeshPrimitive(const tinygltf::Primitive& InPrimitive, const tinygltf::Model& InModel)
{
	Ref<FMeshPrimitiveStorage> OutStorage = CreateRef<FMeshPrimitiveStorage>();
	OutStorage->MaterialIndex = InPrimitive.material;
	OutStorage->PrimitiveTopology = TranslatePrimitiveTopology(InPrimitive.mode);

	for (auto& [attrName, attrIndex] : InPrimitive.attributes)
	{
		REV_CORE_ASSERT(attrIndex >= 0 && attrIndex < InModel.accessors.size());
		const tinygltf::Accessor& RefAccessor = InModel.accessors[attrIndex];
		if (attrName.compare("POSITION") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC3);
			OutStorage->PositonData = LoadBufferData<float, 3>(RefAccessor, InModel);
		}
		/*else if (attrName.compare("COLOR") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC4);
			OutStorage->ColorData = LoadBufferData<float, 4>(RefAccessor, InModel);
		}*/
		else if (attrName.compare("TEXCOORD_0") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC2);
			OutStorage->TexCoordData = LoadBufferData<float, 2>(RefAccessor, InModel);
		}
		else if (attrName.compare("NORMAL") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC3);
			OutStorage->NormalData = LoadBufferData<float, 3>(RefAccessor, InModel);
		}
		else if (attrName.compare("TANGENT") == 0)
		{
			REV_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC4);
			OutStorage->TangentData = LoadBufferData<float, 3>(RefAccessor, InModel);
		}
	}
	OutStorage->VertexCount = OutStorage->PositonData.Size() / 3 / sizeof(float);

	{
		//Load Index Data
		REV_CORE_ASSERT(InPrimitive.indices >= 0 && InPrimitive.indices < InModel.accessors.size());
		const tinygltf::Accessor&  RefAccessor = InModel.accessors[InPrimitive.indices];
		REV_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_SCALAR);
		OutStorage->IndexCount = RefAccessor.count;
		OutStorage->IndexStride = GetComponentSize(RefAccessor.componentType);
		OutStorage->IndexData = LoadBufferData(RefAccessor, InModel, OutStorage->IndexStride);
	}

	return OutStorage;
}

Ref<FStaticMeshStorage> FGLTFUtils::ImportStaticMesh(const tinygltf::Mesh& InMesh, const tinygltf::Model& InModel, const std::vector<Ref<FSurfaceMaterialStorage>>& InMaterials)
{
	std::map<int, int> MaterialIndexMap;

	Ref<FStaticMeshStorage> OutStorage = CreateRef<FStaticMeshStorage>();
	OutStorage->Name = InMesh.name;
	for (size_t i = 0; i < InMesh.primitives.size(); i++) {
		Ref<FMeshPrimitiveStorage> PrimitiveStorage = ImportMeshPrimitive(InMesh.primitives[i], InModel);
		if (PrimitiveStorage)
		{
			if (MaterialIndexMap.count(PrimitiveStorage->MaterialIndex) == 0)
			{
				Ref<FSurfaceMaterialStorage> Mat = PrimitiveStorage->MaterialIndex < 0 ? CreateRef<FPBRMaterialStorage>() : InMaterials[PrimitiveStorage->MaterialIndex];
				int MappedIndex = OutStorage->Materials.size();
				PrimitiveStorage->MaterialIndex = MappedIndex;
				MaterialIndexMap.emplace(PrimitiveStorage->MaterialIndex, MappedIndex);
				OutStorage->Materials.emplace_back(std::move(Mat));
			}
			else
			{
				PrimitiveStorage->MaterialIndex = MaterialIndexMap[PrimitiveStorage->MaterialIndex];
			}
			OutStorage->Primitives.emplace_back(std::move(PrimitiveStorage));
		}
	}
	return OutStorage;
}

Ref<FTextureStorage> FGLTFUtils::ImportTexture(const tinygltf::Texture& InTexture, const tinygltf::Model& InModel)
{
	const tinygltf::Image& InImage = InModel.images[InTexture.source];
	const tinygltf::Sampler& InSampler = InModel.samplers[InTexture.sampler];

	Ref<FTextureStorage> Result = CreateRef<FTextureStorage>();
	if (InTexture.name.empty())
	{
		if (InImage.name.empty())
		{
			std::filesystem::path uri(InImage.uri);
			Result->Name = uri.filename().generic_string();
		}
		else
		{
			Result->Name = InImage.name;
		}
	}
	else
	{
		Result->Name = InTexture.name;
	}
	Result->TextureDesc = FTextureDesc::Make2D(InImage.width, InImage.height, TranslateImageFormat(InImage), false, Math::FLinearColor(0, 0, 0, 1));
	Result->SamplerDesc = TranslateSampler(InSampler);
	{
		Result->ImageData.Resize(1, 1);
		FBuffer& ImageBuffer = Result->ImageData.At(0, 0);
		//Decoded data
		ImageBuffer.Allocate(InImage.image.size());
		memcpy(ImageBuffer.Data(), InImage.image.data(), InImage.image.size());
	}
	//TODO: Decode image data from buffer
	
	return Result;
}

Ref<FSurfaceMaterialStorage> FGLTFUtils::ImportMaterial(const tinygltf::Material& InMaterial, const tinygltf::Model& InModel, const std::vector<Ref<FTextureStorage>>& InTextures)
{
	auto& pbrInfo = InMaterial.pbrMetallicRoughness;
	Ref<FPBRMaterialStorage> Result = CreateRef<FPBRMaterialStorage>();
	Result->Name = InMaterial.name;
	
	{
		//Alpha Mode
		if (InMaterial.alphaMode == "OPAQUE")
		{
			Result->BlendMode = MBM_Opaque;
		}
		else if (InMaterial.alphaMode == "BLEND")
		{
			Result->BlendMode = MBM_Transparent;
		}
		else if (InMaterial.alphaMode == "MASK")
		{
			Result->BlendMode = MBM_Masked;
		}
	}

	Result->MaskClip = InMaterial.alphaCutoff;
	Result->TwoSided = InMaterial.doubleSided;
	Result->BaseColorFactor = TranslateColor(pbrInfo.baseColorFactor);
	Result->BaseColorTexture = pbrInfo.baseColorTexture.index >= 0 ? InTextures[pbrInfo.baseColorTexture.index] : nullptr;
	Result->Metallic = pbrInfo.metallicFactor;
	Result->Roughness = pbrInfo.roughnessFactor;
	Result->MetallicRoughnessTexture = pbrInfo.metallicRoughnessTexture.index >= 0 ? InTextures[pbrInfo.metallicRoughnessTexture.index] : nullptr;
	Result->NormalScale = InMaterial.normalTexture.scale;
	Result->NormalTexture = InMaterial.normalTexture.index >= 0 ? InTextures[InMaterial.normalTexture.index] : nullptr;
	Result->OcclusionStrength = InMaterial.occlusionTexture.strength;
	Result->OcclusionTexture = InMaterial.occlusionTexture.index >= 0 ? InTextures[InMaterial.occlusionTexture.index] : nullptr;
	Result->EmissiveFactor = TranslateVector3(InMaterial.emissiveFactor);
	Result->EmissiveTexture = InMaterial.emissiveTexture.index >= 0 ? InTextures[InMaterial.emissiveTexture.index] : nullptr;

	return Result;
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

	for (size_t i = 0; i < InModel.textures.size(); i++)
	{
		Ref<FTextureStorage> TextureStorage = ImportTexture(InModel.textures[i], InModel);
		Result.Textures.emplace_back(std::move(TextureStorage));
	}

	for (size_t i = 0; i < InModel.materials.size(); i++)
	{
		Ref<FSurfaceMaterialStorage> MaterialStorage = ImportMaterial(InModel.materials[i], InModel, Result.Textures);
		Result.Materials.emplace_back(std::move(MaterialStorage));
	}

	for (size_t i = 0; i < InModel.meshes.size(); i++)
	{
		Ref<FStaticMeshStorage> StaticMeshStorage = ImportStaticMesh(InModel.meshes[i], InModel, Result.Materials);
		Result.StaticMeshes.emplace_back(std::move(StaticMeshStorage));

		//TODO: Import skeleton mesh
	}

	return Result;
}

}
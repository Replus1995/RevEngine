#include "GLTFUtils.h"
#include "Rev/Core/Assert.h"

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
	RE_CORE_ASSERT(InAccessor.bufferView >= 0 && InAccessor.bufferView < InModel.bufferViews.size());
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

EMeshDrawMode FGLTFUtils::GetDrawMode(int InDrawMode)
{
	switch (InDrawMode)
	{
	case TINYGLTF_MODE_POINTS:
		return EMeshDrawMode::POINTS;
	case TINYGLTF_MODE_LINE:
		return EMeshDrawMode::LINE;
	case TINYGLTF_MODE_LINE_LOOP:
		return EMeshDrawMode::LINE_LOOP;
	case TINYGLTF_MODE_LINE_STRIP:
		return EMeshDrawMode::LINE_STRIP;
	case TINYGLTF_MODE_TRIANGLES:
		return EMeshDrawMode::TRIANGLES;
	case TINYGLTF_MODE_TRIANGLE_STRIP:
		return EMeshDrawMode::TRIANGLE_STRIP;
	case TINYGLTF_MODE_TRIANGLE_FAN:
		return EMeshDrawMode::TRIANGLE_FAN;
	default:
		return EMeshDrawMode::TRIANGLES;
	}
}

Ref<FMeshPrimitiveStorage> FGLTFUtils::LoadMeshPrimitive(const tinygltf::Primitive& InPrimitive, const tinygltf::Model& InModel)
{
	Ref<FMeshPrimitiveStorage> OutStorage = CreateRef<FMeshPrimitiveStorage>();
	for (auto& [attrName, attrIndex] : InPrimitive.attributes)
	{
		RE_CORE_ASSERT(attrIndex >= 0 && attrIndex < InModel.accessors.size());
		const tinygltf::Accessor& RefAccessor = InModel.accessors[attrIndex];
		if (attrName.compare("POSITION") == 0)
		{
			RE_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			RE_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC3);
			OutStorage->PositonData = LoadBufferData<float, 3>(RefAccessor, InModel);
		}
		else if (attrName.compare("COLOR") == 0)
		{
			RE_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			RE_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC4);
			OutStorage->ColorData = LoadBufferData<float, 4>(RefAccessor, InModel);
		}
		else if (attrName.compare("TEXCOORD_0") == 0)
		{
			RE_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			RE_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC2);
			OutStorage->TexCoordData = LoadBufferData<float, 2>(RefAccessor, InModel);
		}
		else if (attrName.compare("NORMAL") == 0)
		{
			RE_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			RE_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC3);
			OutStorage->NormalData = LoadBufferData<float, 3>(RefAccessor, InModel);
		}
		else if (attrName.compare("TANGENT") == 0)
		{
			RE_CORE_ASSERT(RefAccessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
			RE_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_VEC4);
			OutStorage->TangentData = LoadBufferData<float, 3>(RefAccessor, InModel);
		}
	}


	{
		//Load Index Data
		RE_CORE_ASSERT(InPrimitive.indices >= 0 && InPrimitive.indices < InModel.accessors.size());
		const tinygltf::Accessor&  RefAccessor = InModel.accessors[InPrimitive.indices];
		RE_CORE_ASSERT(RefAccessor.type == TINYGLTF_TYPE_SCALAR);
		OutStorage->IndexCount = RefAccessor.count;
		OutStorage->IndexStride = GetComponentSize(RefAccessor.componentType);
		OutStorage->IndexData = LoadBufferData(RefAccessor, InModel, OutStorage->IndexStride);
	}

	return OutStorage;
}

}
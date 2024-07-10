#include "Rev/Asset/MeshPrimitiveStorage.h"

namespace Rev
{

FMeshPrimitiveStorage::FMeshPrimitiveStorage()
{
}

FMeshPrimitiveStorage::FMeshPrimitiveStorage(FMeshPrimitiveStorage&& InStorage) noexcept
{
	MaterialIndex = InStorage.MaterialIndex;
	DrawMode = InStorage.DrawMode;

	VertexCount = InStorage.VertexCount;
	PositonData = std::move(InStorage.PositonData);
	ColorData = std::move(InStorage.ColorData);
	TexCoordData = std::move(InStorage.TexCoordData);
	NormalData = std::move(InStorage.NormalData);
	TangentData = std::move(InStorage.TangentData);

	//WeightData = std::move(InStorage.WeightData);
	//WeightIndexData = std::move(InStorage.WeightIndexData);

	IndexCount = InStorage.IndexCount;
	IndexStride = InStorage.IndexStride;
	IndexData = std::move(InStorage.IndexData);
}

}



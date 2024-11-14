#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/RenderCore.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Component/StaticMesh.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"

namespace Rev
{

static constexpr float sPlanePositions[] = {
    -0.5, -0.5, 0.0,
    -0.5, +0.5, 0.0,
    +0.5, +0.5, 0.0,
    +0.5, -0.5, 0.0
};

static constexpr float sPlaneTexCoords[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
};

static constexpr uint16 sPlaneIndices[] = {
    0, 3, 1,
    2, 1, 3
};

class FPlaneGeometry
{
public:
    static Ref<FStaticMesh> Create(const Ref<FMaterial>& InMat)
    {
        FStaticMeshBuilder PlaneBuilder;
        PlaneBuilder.Init(4, 1, 6, false);
        PlaneBuilder.FillPositions(sPlanePositions, ARRAY_LENGTH(sPlanePositions));
        PlaneBuilder.FillTexCoords(0, sPlaneTexCoords, ARRAY_LENGTH(sPlaneTexCoords));
        PlaneBuilder.FillIndices(sPlaneIndices, ARRAY_LENGTH(sPlaneIndices));
        PlaneBuilder.SetMaterials({ InMat });

        return PlaneBuilder.Build(true, true);
    }
};

}
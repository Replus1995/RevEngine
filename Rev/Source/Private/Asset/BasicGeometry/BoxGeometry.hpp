#pragma once
#include "Rev/Core/Base.h"
#include "Rev/Render/Material/Material.h"
#include "Rev/Render/Component/StaticMesh.h"
#include "Rev/Render/RHI/RHIBuffer.h"
#include "Rev/Render/RHI/DynamicRHI.h"
#include "Rev/Render/RHI/RHIContext.h"

namespace Rev
{


static constexpr float sBoxPositions[] = {
    -0.5, -0.5, -0.5,
    -0.5, +0.5, -0.5,
    +0.5, +0.5, -0.5,
    +0.5, -0.5, -0.5,
    -0.5, -0.5, +0.5,
    -0.5, +0.5, +0.5,
    +0.5, +0.5, +0.5,
    +0.5, -0.5, +0.5
};

static constexpr uint16 sBoxIndices[] = {
    4, 7, 5,
    6, 5, 7,
    7, 4, 3,
    0, 3, 4,
    6, 7, 2,
    3, 2, 7,
    5, 6, 1,
    2, 1, 6,
    4, 5, 0,
    1, 0, 5,
    0, 1, 3,
    2, 3, 1
};

class FBoxGeometry
{
public:
    static Ref<FStaticMesh> Create(const Ref<FMaterial>& InMat)
    {
        FStaticMeshBuilder BoxBuilder;
        BoxBuilder.Init(8, 1, 36, false);
        BoxBuilder.FillPositions(sBoxPositions, ARRAY_LENGTH(sBoxPositions));
        BoxBuilder.FillIndices(sBoxIndices, ARRAY_LENGTH(sBoxIndices));
        BoxBuilder.SetMaterials({ InMat });

        return BoxBuilder.Build(false, false);
    }
};

}
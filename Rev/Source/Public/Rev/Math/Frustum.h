#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <array>

namespace Rev
{
namespace Math
{

/** Extracts the 8 world-space corners of a view frustum from its inverse view-projection matrix.
 *  Corners are returned in order: near plane (0-3), far plane (4-7), each in counter-clockwise winding.
 *  @param InvViewProj  Inverse of (ViewProj) = InvProj * InvView
 *  @return 8 world-space corner positions
 */
inline std::array<FVector3, 8> GetFrustumCorners(const FMatrix4& InvViewProj)
{
    // NDC cube corners: [-1,1]^3 for standard Vulkan/OpenGL (no reversed-Z)
    std::array<FVector4, 8> NdcCorners = {{
        // Near plane (z = -1 in Vulkan NDC)
        FVector4(-1.0f, -1.0f, -1.0f, 1.0f),
        FVector4( 1.0f, -1.0f, -1.0f, 1.0f),
        FVector4(-1.0f,  1.0f, -1.0f, 1.0f),
        FVector4( 1.0f,  1.0f, -1.0f, 1.0f),
        // Far plane (z = 1 in Vulkan NDC)
        FVector4(-1.0f, -1.0f,  1.0f, 1.0f),
        FVector4( 1.0f, -1.0f,  1.0f, 1.0f),
        FVector4(-1.0f,  1.0f,  1.0f, 1.0f),
        FVector4( 1.0f,  1.0f,  1.0f, 1.0f),
    }};

    std::array<FVector3, 8> WorldCorners;
    for (int i = 0; i < 8; ++i)
    {
        FVector4 World = InvViewProj * NdcCorners[i];
        WorldCorners[i] = FVector3(World.X, World.Y, World.Z) / World.W;
    }
    return WorldCorners;
}

/** Extracts frustum corners for a sub-range of the depth (split near/far). */
inline std::array<FVector3, 8> GetFrustumCorners(const FMatrix4& InvViewProj, float SplitNear, float SplitFar)
{
    // Map split distances to NDC z range [-1, 1]
    // Assumes standard Vulkan projection: near maps to 0, far maps to 1
    // NDC z = -1 for near, +1 for far
    float NdcNear = -1.0f;
    float NdcFar = 1.0f;

    // For a sub-split, we need the full frustum corners and then interpolate
    auto FullCorners = GetFrustumCorners(InvViewProj);
    std::array<FVector3, 8> SplitCorners;

    // Interpolate between near and far planes
    // Near corners: full near plane
    // Far corners: full far plane
    // We use the actual split distances (0 = near plane, 1 = far plane)
    // to lerp along the frustum edges
    auto NearPlane = [&FullCorners](float T)
    {
        std::array<FVector3, 4> Result;
        for (int i = 0; i < 4; ++i)
            Result[i] = FVector3::Lerp(FullCorners[i], FullCorners[i + 4], T);
        return Result;
    };

    // In standard projection: near at z=0, far at z=1 (after perspective divide)
    // The corners are already in world space with full near/far range
    // For a sub-split, we find the fractional positions along frustum edges
    // We need to reverse the projection mapping: depth = (far*near) / (far - depthWindow*(far-near))
    // For simplicity, use linear interpolation in world space along frustum edges
    auto SplitNearPlane = NearPlane(SplitNear);
    auto SplitFarPlane = NearPlane(SplitFar);

    for (int i = 0; i < 4; ++i)
    {
        SplitCorners[i] = SplitNearPlane[i];
        SplitCorners[i + 4] = SplitFarPlane[i];
    }
    return SplitCorners;
}

} // namespace Math
} // namespace Rev

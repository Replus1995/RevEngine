#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include "BoundingBox.h"
#include "Frustum.h"
#include <array>
#include <vector>

namespace Rev
{
namespace Math
{

/**
 * Calculates cascade split distances using the Practical Split Scheme (PSSM).
 * @param Near         Camera near plane distance
 * @param Far          Camera far plane distance
 * @param NumCascades  Number of cascades (typically 4)
 * @param Lambda       Blend factor: 0 = uniform, 1 = logarithmic (0.5-0.75 recommended)
 * @return Array of split distances (NumCascades + 1 entries: [near, split1, split2, ..., far])
 */
inline std::vector<float> CalculateCascadeSplits(float Near, float Far, uint32 NumCascades, float Lambda = 0.5f)
{
    std::vector<float> Splits(NumCascades + 1);
    Splits[0] = Near;
    Splits[NumCascades] = Far;

    for (uint32 i = 1; i < NumCascades; ++i)
    {
        float Fraction = float(i) / float(NumCascades);
        float LogSplit = Near * std::pow(Far / Near, Fraction);
        float UniformSplit = Near + (Far - Near) * Fraction;
        Splits[i] = Lambda * LogSplit + (1.0f - Lambda) * UniformSplit;
    }

    return Splits;
}

/**
 * Calculates the light view-projection matrix for a single cascade.
 *
 * Builds an orthographic projection that tightly bounds the frustum sub-section
 * as seen from the light's direction. This minimizes wasted shadow map texels.
 *
 * @param WorldCorners  The 8 world-space corners of the cascade frustum
 * @param LightDir      Directional light direction (world space, normalized)
 * @param LightViewProj [out] The resulting light view-projection matrix
 */
inline void CalculateLightViewProj(
    const std::array<FVector3, 8>& WorldCorners,
    const FVector3& LightDir,
    FMatrix4& OutLightViewProj)
{
    // Build light view matrix: look from the center of the cascade in light direction
    FVector3 Center(0.0f);
    for (const auto& Corner : WorldCorners)
        Center += Corner;
    Center /= 8.0f;

    // Place the "camera" far enough back to cover the entire cascade
    // Use the light direction to position the view point
    FVector3 Eye = Center - LightDir * 100.0f; // arbitrary large offset along light dir

    // Look-at matrix from light's perspective
    FVector3 LookTarget = Center;
    FVector3 WorldUp = FVector3(0.0f, 1.0f, 0.0f);
    // Avoid degenerate case when light is parallel to world up
    if (std::abs(FVector3::Dot(LightDir, WorldUp)) > 0.999f)
        WorldUp = FVector3(0.0f, 0.0f, 1.0f);

    FVector3 ZAxis = LightDir.Normalized();
    FVector3 XAxis = FVector3::Cross(WorldUp, ZAxis).Normalized();
    FVector3 YAxis = FVector3::Cross(ZAxis, XAxis);

    // Build view matrix (column-major)
    FMatrix4 LightViewMat(
        FVector4(XAxis.X, YAxis.X, ZAxis.X, 0.0f),
        FVector4(XAxis.Y, YAxis.Y, ZAxis.Y, 0.0f),
        FVector4(XAxis.Z, YAxis.Z, ZAxis.Z, 0.0f),
        FVector4(-FVector3::Dot(XAxis, Eye), -FVector3::Dot(YAxis, Eye), -FVector3::Dot(ZAxis, Eye), 1.0f)
    );

    // Transform all corners into light view space to compute tight ortho bounds
    FBox LightSpaceBox;
    for (const auto& Corner : WorldCorners)
    {
        FVector4 LightSpace = LightViewMat * FVector4(Corner.X, Corner.Y, Corner.Z, 1.0f);
        LightSpaceBox.Expand(FVector3(LightSpace.X, LightSpace.Y, LightSpace.Z));
    }

    // Add padding to avoid edge artifacts
    const float Padding = 10.0f;
    LightSpaceBox.Min.Z -= Padding; // Extend far plane
    LightSpaceBox.Max.Z += Padding; // Extend near plane

    // Build orthographic projection for this cascade
    FMatrix4 LightProj = FMatrix4::Othographic(
        LightSpaceBox.Min.X, LightSpaceBox.Max.X,
        LightSpaceBox.Min.Y, LightSpaceBox.Max.Y,
        -LightSpaceBox.Max.Z, -LightSpaceBox.Min.Z  // Vulkan: 0=near, 1=far (reverse Z)
    );

    OutLightViewProj = LightProj * LightViewMat;
}

} // namespace Math
} // namespace Rev

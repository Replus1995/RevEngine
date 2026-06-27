#pragma once
#include "Vector3.h"
#include "Matrix4.h"
#include <array>

namespace Rev
{
namespace Math
{

/** Axis-aligned bounding box. */
struct FBox
{
    FVector3 Min = FVector3(FLT_MAX);
    FVector3 Max = FVector3(-FLT_MAX);

    FBox() = default;
    FBox(const FVector3& InMin, const FVector3& InMax) : Min(InMin), Max(InMax) {}

    FORCEINLINE FVector3 GetCenter() const { return (Min + Max) * 0.5f; }
    FORCEINLINE FVector3 GetExtent() const { return (Max - Min) * 0.5f; }
    FORCEINLINE bool IsValid() const { return Min.X <= Max.X && Min.Y <= Max.Y && Min.Z <= Max.Z; }

    void Expand(const FVector3& InPoint)
    {
        Min.X = std::min(Min.X, InPoint.X);
        Min.Y = std::min(Min.Y, InPoint.Y);
        Min.Z = std::min(Min.Z, InPoint.Z);
        Max.X = std::max(Max.X, InPoint.X);
        Max.Y = std::max(Max.Y, InPoint.Y);
        Max.Z = std::max(Max.Z, InPoint.Z);
    }

    /** Returns the 8 corners of the AABB in world space. */
    std::array<FVector3, 8> GetCorners() const
    {
        return {
            FVector3(Min.X, Min.Y, Min.Z),
            FVector3(Max.X, Min.Y, Min.Z),
            FVector3(Min.X, Max.Y, Min.Z),
            FVector3(Max.X, Max.Y, Min.Z),
            FVector3(Min.X, Min.Y, Max.Z),
            FVector3(Max.X, Min.Y, Max.Z),
            FVector3(Min.X, Max.Y, Max.Z),
            FVector3(Max.X, Max.Y, Max.Z),
        };
    }

    /** Transform an AABB by a matrix and return the new enclosing AABB. */
    static FBox Transform(const FBox& Box, const FMatrix4& Mat)
    {
        auto Corners = Box.GetCorners();
        FBox Result;
        for (const auto& Corner : Corners)
        {
            FVector4 Transformed = Mat * FVector4(Corner.X, Corner.Y, Corner.Z, 1.0f);
            Result.Expand(FVector3(Transformed.X, Transformed.Y, Transformed.Z) / Transformed.W);
        }
        return Result;
    }
};

} // namespace Math
} // namespace Rev

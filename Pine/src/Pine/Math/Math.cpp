#include "pinepch.h"
#include "Pine/Math/Math.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Pine
{
//Current euler angle rotate order is x->y->z, to be changed into roll->pitch->yaw in future


float FMaths::Radians(float degree)
{
    return glm::radians(degree);
}

float FMaths::Degrees(float radian)
{
    return glm::degrees(radian);
}

FRotator FMaths::MakeRotator(const FQuaternion& q)
{
    float pitch = glm::degrees(glm::pitch(q));
    float yaw = glm::degrees(glm::yaw(q));
    float roll = glm::degrees(glm::roll(q));
    return FRotator(pitch, yaw, roll);
}

FQuaternion FMaths::MakeQuaternion(const FRotator& r)
{
    return glm::quat(glm::radians(glm::vec3(r.pitch, r.yaw, r.roll)));
}

FMatrix4 FMaths::MakeMatrix(const FRotator& r)
{
    return glm::eulerAngleXYZ(glm::radians(r.pitch), glm::radians(r.yaw), glm::radians(r.roll));
}

FMatrix4 FMaths::MakeMatrix(const FQuaternion& q)
{
    return glm::toMat4(q);
}

FMatrix4 FMaths::MakeMatrix(const FVector3& t, const FRotator& r, const FVector3& s)
{
    return glm::translate(glm::mat4(1.0f), t) * MakeMatrix(r)  * glm::scale(glm::mat4(1.0f), s);
}

FMatrix4 FMaths::MakeMatrix(const FVector3& t, const FQuaternion& q, const FVector3& s)
{
    return glm::translate(glm::mat4(1.0f), t) * MakeMatrix(q) * glm::scale(glm::mat4(1.0f), s);
}

bool FMaths::Decompose(const FMatrix4& transform, FVector3& translation, FRotator& rotator, FVector3& scale)
{
    // From glm::decompose in matrix_decompose.inl

    using namespace glm;
    using T = float;

    mat4 LocalMatrix(transform);

    // Normalize the matrix.
    if (epsilonEqual(LocalMatrix[3][3], static_cast<float>(0), epsilon<T>()))
        return false;

    // First, isolate perspective.  This is the messiest.
    if (
        epsilonNotEqual(LocalMatrix[0][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[1][3], static_cast<T>(0), epsilon<T>()) ||
        epsilonNotEqual(LocalMatrix[2][3], static_cast<T>(0), epsilon<T>()))
    {
        // Clear the perspective partition
        LocalMatrix[0][3] = LocalMatrix[1][3] = LocalMatrix[2][3] = static_cast<T>(0);
        LocalMatrix[3][3] = static_cast<T>(1);
    }

    // Next take care of translation (easy).
    translation = vec3(LocalMatrix[3]);
    LocalMatrix[3] = vec4(0, 0, 0, LocalMatrix[3].w);

    vec3 Row[3], Pdum3;

    // Now get scale and shear.
    for (length_t i = 0; i < 3; ++i)
        for (length_t j = 0; j < 3; ++j)
            Row[i][j] = LocalMatrix[i][j];

    // Compute X scale factor and normalize first row.
    scale.x = length(Row[0]);
    Row[0] = detail::scale(Row[0], static_cast<T>(1));
    scale.y = length(Row[1]);
    Row[1] = detail::scale(Row[1], static_cast<T>(1));
    scale.z = length(Row[2]);
    Row[2] = detail::scale(Row[2], static_cast<T>(1));

    // At this point, the matrix (in rows[]) is orthonormal.
    // Check for a coordinate system flip.  If the determinant
    // is -1, then negate the matrix and the scaling factors.
#if 0
    Pdum3 = cross(Row[1], Row[2]); // v3Cross(row[1], row[2], Pdum3);
    if (dot(Row[0], Pdum3) < 0)
    {
        for (length_t i = 0; i < 3; i++)
        {
            scale[i] *= static_cast<T>(-1);
            Row[i] *= static_cast<T>(-1);
        }
    }
#endif

    glm::vec3 rotation;

    rotation.y = asin(-Row[0][2]);
    if (cos(rotation.y) != 0) {
        rotation.x = atan2(Row[1][2], Row[2][2]);
        rotation.z = atan2(Row[0][1], Row[0][0]);
    }
    else {
        rotation.x = atan2(-Row[2][0], Row[1][1]);
        rotation.z = 0;
    }

    rotator.pitch = glm::degrees(rotation.x);
    rotator.yaw = glm::degrees(rotation.y);
    rotator.roll = glm::degrees(rotation.z);

    return true;
}

FVector3 FMaths::Right(const FRotator& r)
{
    return glm::rotate(MakeQuaternion(r), FVector3(1.0f, 0.0f, 0.0f));
}

FVector3 FMaths::Up(const FRotator& r)
{
    return glm::rotate(MakeQuaternion(r), FVector3(0.0f, 1.0f, 0.0f));
}

FVector3 FMaths::Forward(const FRotator& r)
{
    return glm::rotate(MakeQuaternion(r), FVector3(0.0f, 0.0f, -1.0f));
}

float FMaths::Clamp(float val, float minVal, float maxVal)
{
    return val < minVal ? minVal : (val > maxVal ? maxVal : val);
}

}
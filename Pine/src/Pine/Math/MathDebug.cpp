#include "pinepch.h"
#include "MathDebug.h"

std::ostream& operator<<(std::ostream& os, const Pine::FMatrix3 mat)
{
    os << "[\n";
    for (size_t i = 0; i < 3; i++)
    {
        os << mat[0][i] << "," << mat[1][i] << "," << mat[2][i] << ";\n";
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Pine::FMatrix4 mat)
{
    os << "[\n";
    for (size_t i = 0; i < 4; i++)
    {
        os << mat[0][i] << "," << mat[1][i] << "," << mat[2][i] << "," << mat[3][i] << ";\n";
    }
    os << "]";
    return os;
}

#pragma once
#include "Maths.h"
#include <iostream>

template<typename T>
std::ostream& operator<<(std::ostream& os, const Rev::Math::TVector2<T>& vec)
{
	os << "(" << vec.X << ", " << vec.Y << ")";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Rev::Math::TVector3<T>& vec)
{
	os << "(" << vec.X << ", " << vec.Y << ", " << vec.Z << ")";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Rev::Math::TVector4<T>& vec)
{
	os << "(" << vec.X << ", " << vec.Y << ", " << vec.Z << ", " << vec.W << ")";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Rev::Math::TRotator<T>& rot)
{
	os << "(" << rot.Pitch << ", " << rot.Yaw << ", " << rot.Roll << ")";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Rev::Math::TQuaternion<T>& vec)
{
	os << "(" << vec.X << ", " << vec.Y << ", " << vec.Z << ", " << vec.W << ")";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Rev::Math::FMatrix3 mat)
{
	os << "[\n";
	for (size_t i = 0; i < 3; i++)
	{
		os << mat[0][i] << "," << mat[1][i] << "," << mat[2][i] << ";\n";
	}
	os << "]";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Rev::Math::FMatrix4 mat)
{
	os << "[\n";
	for (size_t i = 0; i < 4; i++)
	{
		os << mat[0][i] << "," << mat[1][i] << "," << mat[2][i] << "," << mat[3][i] << ";\n";
	}
	os << "]";
	return os;
}
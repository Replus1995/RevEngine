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

template<typename T>
std::ostream& operator<<(std::ostream& os, const Rev::Math::TMatrix3<T> mat)
{
	os << "[\n";
	for (int i = 0; i < 3; i++)
	{
		os << mat[0][i] << "," << mat[1][i] << "," << mat[2][i] << ";\n";
	}
	os << "]";
	return os;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const Rev::Math::TMatrix4<T> mat)
{
	os << "[\n";
	for (int i = 0; i < 4; i++)
	{
		os << mat[0][i] << "," << mat[1][i] << "," << mat[2][i] << "," << mat[3][i] << ";\n";
	}
	os << "]";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Rev::Math::FColor& c)
{
	os << "(" << (int)c.R << ", " << (int)c.G << ", " << (int)c.B << ", " << (int)c.A << ")";
	return os;
}

inline std::ostream& operator<<(std::ostream& os, const Rev::Math::FLinearColor& c)
{
	os << "(" << c.R << ", " << c.G << ", " << c.B << ", " << c.A << ")";
	return os;
}
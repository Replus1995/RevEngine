#pragma once
#include "Maths.h"
#include <iostream>

template<typename T>
std::ostream& operator<<(std::ostream& os, const Pine::TVector2<T>& vec)
{
	os << "(" << vec.X << ", " << vec.Y << ")";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Pine::TVector3<T>& vec)
{
	os << "(" << vec.X << ", " << vec.Y << ", " << vec.Z << ")";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Pine::TVector4<T>& vec)
{
	os << "(" << vec.X << ", " << vec.Y << ", " << vec.Z << ", " << vec.W << ")";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Pine::TRotator<T>& rot)
{
	os << "(" << rot.Pitch << ", " << rot.Yaw << ", " << rot.Roll << ")";
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Pine::TQuaternion<T>& vec)
{
	os << "(" << vec.X << ", " << vec.Y << ", " << vec.Z << ", " << vec.W << ")";
	return os;
}

std::ostream& operator<<(std::ostream& os, const Pine::FMatrix3 mat);
std::ostream& operator<<(std::ostream& os, const Pine::FMatrix4 mat);
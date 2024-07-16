#pragma once
#include "MathUtils.h"
#include <cassert>
#include <cmath>

namespace Rev
{
namespace Math
{

template<typename T>
inline TMatrix2<T>::TMatrix2()
{
}

template<typename T>
inline TMatrix2<T>::TMatrix2(T InScalar)
{
	Columns[0] = TVector2<T>(InScalar, 0.0F);
	Columns[1] = TVector2<T>(0.0F, InScalar);
}

template<typename T>
inline TMatrix2<T>::TMatrix2(TVector2<T> InCol0, TVector2<T> InCol1)
{
	Columns[0] = InCol0;
	Columns[1] = InCol1;
}

template<typename T>
inline TVector2<T>& TMatrix2<T>::operator[](int Index)
{
	assert((Index >= 0 && Index < 3) && "Matrix3 index out of range");
	return Columns[Index];
}

template<typename T>
inline TVector2<T> const& TMatrix2<T>::operator[](int Index) const
{
	assert((Index >= 0 && Index < 3) && "Matrix3 index out of range");
	return Columns[Index];
}

template<typename T>
inline T const* TMatrix2<T>::Data() const
{
	return &(Columns[0].X);
}

template<typename T>
inline TMatrix2<T>& TMatrix2<T>::operator=(const TMatrix2<T>& InMat)
{
	Columns[0] = InMat[0];
	Columns[1] = InMat[1];
	return *this;
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::operator+(const TMatrix2<T>& InMat) const
{
	return TMatrix2<T>(Columns[0] + InMat[0], Columns[1] + InMat[1]);
}

template<typename T>
inline TMatrix2<T>& TMatrix2<T>::operator+=(const TMatrix2<T>& InMat)
{
	Columns[0] += InMat[0];
	Columns[1] += InMat[1];
	return *this;
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::operator-(const TMatrix2<T>& InMat) const
{
	return TMatrix2<T>(Columns[0] - InMat[0], Columns[1] - InMat[1]);
}

template<typename T>
inline TMatrix2<T>& TMatrix2<T>::operator-=(const TMatrix2<T>& InMat)
{
	Columns[0] -= InMat[0];
	Columns[1] -= InMat[1];
	return *this;
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::operator*(const TMatrix2<T>& InMat) const
{
	TMatrix2<T> const& m1 = *this;
	TMatrix2<T> const& m2 = InMat;

	//From GLM
	T const SrcA00 = m1[0][0];
	T const SrcA01 = m1[0][1];
	T const SrcA10 = m1[1][0];
	T const SrcA11 = m1[1][1];

	T const SrcB00 = m2[0][0];
	T const SrcB01 = m2[0][1];
	T const SrcB10 = m2[1][0];
	T const SrcB11 = m2[1][1];

	TMatrix2<T> Result;
	Result[0][0] = SrcA00 * SrcB00 + SrcA10 * SrcB01;
	Result[0][1] = SrcA01 * SrcB00 + SrcA11 * SrcB01;
	Result[1][0] = SrcA00 * SrcB10 + SrcA10 * SrcB11;
	Result[1][1] = SrcA01 * SrcB10 + SrcA11 * SrcB11;
	return Result;
}

template<typename T>
inline TMatrix2<T>& TMatrix2<T>::operator*=(const TMatrix2<T>& InMat)
{
	return (*this = *this * InMat);
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::operator*(T InScalar) const
{
	return TMatrix2<T>(Columns[0] * InScalar, Columns[1] * InScalar);
}

template<typename T>
inline TMatrix2<T>& TMatrix2<T>::operator*=(T InScalar)
{
	Columns[0] *= InScalar;
	Columns[1] *= InScalar;
	return *this;
}

template<typename T>
inline TVector2<T> TMatrix2<T>::operator*(const TVector2<T>& InVec) const
{
	TVector2<T> Result;
	Result[0] = Columns[0][0] * InVec[0] + Columns[0][1] * InVec[1];
	Result[1] = Columns[1][0] * InVec[0] + Columns[1][1] * InVec[1];
	return Result;
}

template<typename T>
inline void TMatrix2<T>::Invert()
{
	*this = Inverse();
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::Inverse() const
{
	//From GLM
	T OneOverDeterminant = 1.0F / (
		+ Columns[0][0] * Columns[1][1]
		- Columns[1][0] * Columns[0][1]);

	TMatrix2<T> Inverse;
	Inverse[0][0] = + Columns[1][1] * OneOverDeterminant;
	Inverse[1][0] = - Columns[1][0] * OneOverDeterminant;
	Inverse[0][1] = - Columns[0][1] * OneOverDeterminant;
	Inverse[1][1] = + Columns[0][0] * OneOverDeterminant;

	return Inverse;
}

template<typename T>
inline void TMatrix2<T>::Transpose()
{
	*this = Transposed();
}

template<typename T>
inline TMatrix2<T> TMatrix2<T>::Transposed() const
{
	TMatrix2<T> Transposed;
	Transposed[0][0] = Columns[0][0];
	Transposed[0][1] = Columns[1][0];
	Transposed[1][0] = Columns[0][1];
	Transposed[1][1] = Columns[1][1];
	return Transposed;
}

}
}
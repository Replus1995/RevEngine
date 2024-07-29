#pragma once
#include "Rev/Core/Base.h"

namespace Rev
{

//TODO: Byte Order

class REV_API FArchive
{
public:
	virtual ~FArchive() = default;

	virtual bool IsLoading() const = 0;
	virtual void Serialize(void* pValue, int64 Length) {}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, uint8& Value)
	{
		Ar.Serialize(&Value, sizeof(uint8));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, int8& Value)
	{
		Ar.Serialize(&Value, sizeof(int8));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, uint16& Value)
	{
		Ar.Serialize(&Value, sizeof(uint16));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, int16& Value)
	{
		Ar.Serialize(&Value, sizeof(int16));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, uint32& Value)
	{
		Ar.Serialize(&Value, sizeof(uint32));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, int32& Value)
	{
		Ar.Serialize(&Value, sizeof(int32));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, uint64& Value)
	{
		Ar.Serialize(&Value, sizeof(uint64));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, int64& Value)
	{
		Ar.Serialize(&Value, sizeof(int64));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, float& Value)
	{
		Ar.Serialize(&Value, sizeof(float));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, double& Value)
	{
		Ar.Serialize(&Value, sizeof(double));
		return Ar;
	}

	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, bool& Value)
	{
		Ar.Serialize(&Value, sizeof(bool));
		return Ar;
	}

	template<typename TEnumType, ENABLE_CHECK(TIsEnum<TEnumType>)>
	FORCEINLINE friend FArchive& operator<<(FArchive& Ar, TEnumType& Value)
	{
		Ar << (std::underlying_type<TEnumType>::type&)Value;
		return Ar;
	}

	friend FArchive& operator<<(FArchive& Ar, std::string& Value);

protected:
	FArchive() = default;
};


}
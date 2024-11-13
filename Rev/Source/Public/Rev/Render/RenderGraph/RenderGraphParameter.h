#pragma once
#include "Rev/Core/Base.h"
#include <typeindex>

namespace Rev
{

class REV_API FRGParameterStruct
{
public:
	template <typename ParameterType>
	explicit FRGParameterStruct(const ParameterType* Parameters)
		: Content(reinterpret_cast<const uint8*>(Parameters))
		, ContentTypeID(typeid(ParameterType))
	{}

	explicit FRGParameterStruct(const void* InContent, std::type_index InTypeID)
		: Content(reinterpret_cast<const uint8*>(InContent))
		, ContentTypeID(InTypeID)
	{}

	const uint8* GetContent() const { return Content; }

	template<typename ParameterType>
	const ParameterType* GetContentAs() const
	{
		if (typeid(ParameterType) == ContentTypeID)
		{
			return reinterpret_cast<const ParameterType*>(FRGParameterStruct::GetContent());
		}
		return nullptr;
	}

private:
	const uint8* Content;
	std::type_index ContentTypeID;

	friend class FRGPass;
};

template<typename ParameterType>
class TRGParameterStruct : public FRGParameterStruct
{
public:
	explicit TRGParameterStruct(ParameterType* Parameters)
		: FRGParameterStruct(Parameters)
	{}

	const ParameterType* GetContent() const
	{
		return reinterpret_cast<const ParameterType*>(FRGParameterStruct::GetContent());
	}

	const ParameterType* operator->() const
	{
		return GetContent();
	}
};


}
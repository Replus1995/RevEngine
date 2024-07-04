#pragma once
#include "Rev/Core/Base.h"
#include <memory>

namespace Rev
{

class REV_API UUID
{
public:
	UUID();
	UUID(uint64_t uuid);
	UUID(const UUID&) = default;

	operator uint64_t() const { return mUUID; }
private:
	uint64_t mUUID;
};

}

namespace std
{

template <typename T> struct hash;
template<>
struct hash<Rev::UUID>
{
	std::size_t operator()(const Rev::UUID& uuid) const
	{
		return (uint64_t)uuid;
	}
};

}
#pragma once
#include "Pine/Core/Base.h"
#include <memory>

namespace Pine
{

class PINE_API UUID
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
struct hash<Pine::UUID>
{
	std::size_t operator()(const Pine::UUID& uuid) const
	{
		return (uint64_t)uuid;
	}
};

}
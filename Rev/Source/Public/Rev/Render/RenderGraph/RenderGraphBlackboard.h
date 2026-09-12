#pragma once
#include "Rev/Core/Base.h"
#include <any>
#include <typeindex>
#include <unordered_map>

namespace Rev
{
class REV_API FRGBlackboard
{
public:
	template<class T, class... Args> T& Emplace(Args&&... InArgs)
	{
		auto& Value = Values[std::type_index(typeid(T))];
		Value = T(std::forward<Args>(InArgs)...);
		return std::any_cast<T&>(Value);
	}
	template<class T> T* Get()
	{
		auto It = Values.find(std::type_index(typeid(T)));
		return It == Values.end() ? nullptr : std::any_cast<T>(&It->second);
	}
private:
	std::unordered_map<std::type_index, std::any> Values;
};
}

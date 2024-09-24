#pragma once
#include "Rev/Core/CoreDefines.h"
#include "Rev/Core/CoreTypes.h"
#include <string>
#include <sstream>
#include <vector>

namespace Rev
{

template<typename T>
using Scope = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Scope<T> CreateScope(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Ref = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Ref<T> CreateRef(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

}



//#ifndef GLM_ENABLE_EXPERIMENTAL
//	#define GLM_ENABLE_EXPERIMENTAL
//#endif // !GLM_ENABLE_EXPERIMENTAL

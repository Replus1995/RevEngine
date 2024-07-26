#pragma once
#include <memory>
#include <type_traits>

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

namespace Rev
{

template <class T>
using TIsIntegral = std::is_integral<T>;

template <class T>
using TIsFloatingPoint = std::is_floating_point<T>;

template <class T>
using TIsEnum = std::is_enum<T>;

template <class T>
using TIsClass = std::is_class<T>;

template <class TBase, class TDerived>
using TIsBaseOf = std::is_base_of<TBase, TDerived>;

template <class T>
using TIsPointer = std::is_pointer<T>;

template <class T>
using TIsPod = std::is_pod<T>;

template <class T>
using TIsArray = std::is_array<T>;

template <bool Predicate, class Result = void>
using TEnableIf = std::enable_if<Predicate, Result>;

template <typename... Condition>
using TConjunction = std::conjunction<Condition...>;

template <typename... Condition>
using TDisjunction = std::disjunction<Condition...>;

}

#define ENABLE_CHECK(...) class = typename Rev::TEnableIf<Rev::TConjunction<__VA_ARGS__>::value>::type 

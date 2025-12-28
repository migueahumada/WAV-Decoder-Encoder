#pragma once
#include <stdint.h>
#include <string>
#include <array>
#include <map>
#include <vector>


#define NODISCARD_MSG "This value is not being used!"
#define NODISCARD [[nodiscard(NODISCARD_MSG)]]

constexpr float PI = 3.141592f;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using String = std::string;

template<typename K, typename V>
using Map = std::map<K,V>;

template<class T, size_t N>
using Array = std::array<T, N>;

template<class T>
using Vector = std::vector<T>;

template<typename Num>
Num clamp(Num value, Num min, Num max)
{
	static_assert(std::is_arithmetic<Num>::value);

	if (value >= max) return max;
	if (value <= min) return min;

	return value;

}




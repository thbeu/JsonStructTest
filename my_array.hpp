#pragma once

#include "nlohmann/json.hpp"
#include <ostream>

namespace AS {
	template<typename T, size_t N>
	struct my_array {
		T data[N];

		T& operator[](std::size_t i) {
			return data[i];
		}
		const T& operator[](std::size_t i) const {
			return data[i];
		}
	};

	template<typename T, size_t N>
	inline void to_json(nlohmann::json& j, const my_array<T, N>& arr) {
		j = nlohmann::json::array();
		for (size_t i = 0; i < N; ++i) {
			j.push_back(arr.data[i]);
		}
	}

	template<typename T, size_t N>
	inline void from_json(const nlohmann::json& j, my_array<T, N>& arr) {
		for (size_t i = 0; i < N; ++i)
		{
			arr.data[i] = j.at(i).get<T>();
		}
	}
}

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const AS::my_array<T, N>& arr) {
	os << "[";
	for (size_t i = 0; i < N; ++i) {
		if (i > 0) {
			os << ", ";
		}
		os << arr[i];
	}
	return os << "]";
}

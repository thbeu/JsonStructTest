#pragma once

#include "nlohmann/json.hpp"
#include <cinttypes>
#include <ostream>
#include <string>

namespace AS {
	struct my_guid {
		uint32_t data[4] = { 1,2,3,4 };

		std::string to_string() const {
			return "1-2-3-4";
		}

		static my_guid from_string(const std::string& str) {
			my_guid g;
			// parse from UUID string
			return g;
		}
	};

	inline void to_json(nlohmann::json& j, const my_guid& g) {
		j = g.to_string();
	}

	inline void from_json(const nlohmann::json& j, my_guid& g) {
		g = my_guid::from_string(j.get<std::string>());
	}
}

std::ostream& operator<<(std::ostream& os, const AS::my_guid& guid) {
	return os << guid.to_string();
}

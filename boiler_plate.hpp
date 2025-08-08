#pragma once

#include "nlohmann/json.hpp"
#include "visit_struct/visit_struct.hpp"
#include <optional>
#include <ostream>
#include <type_traits>
#include <variant>

namespace nlohmann {
	template <typename... Ts>
	struct adl_serializer<std::variant<Ts...>> {
		static void to_json(json& j, const std::variant<Ts...>& v) {
			std::visit([&](auto&& arg) {
				j = arg;
			}, v);
		}
		static void from_json(const json& j, std::variant<Ts...>& v) {
			from_json_impl<0, Ts...>(j, v);
		}

	private:
		template <std::size_t I, typename First, typename... Rest>
		static void from_json_impl(const json& j, std::variant<Ts...>& v) {
			try {
				if constexpr (std::is_integral<First>::value) {
					if (j.is_number_integer()) {
						v = j.get<First>();
						return;
					}
				}
				else if constexpr (std::is_floating_point<First>::value) {
					if (j.is_number_float()) {
						v = j.get<First>();
						return;
					}
				}
				else {
					v = j.get<First>();
					return;
				}
			}
			catch (const json::exception&) {}

			if constexpr (sizeof...(Rest) > 0) {
				from_json_impl<I + 1, Rest...>(j, v);
			}
			else {
				throw std::runtime_error("Could not deserialize JSON to any type in the variant");
			}
		}
	};
}

namespace AS {
	template <typename T>
	struct is_optional : std::false_type {};

	template <typename T>
	struct is_optional<std::optional<T>> : std::true_type {};

	// Generic to_json for visitable structs in namespace AS
	template<typename T>
	typename std::enable_if<visit_struct::traits::is_visitable<T>::value>::type
		to_json(nlohmann::json& j, const T& value) {
		visit_struct::for_each(value, [&](const char* name, const auto& field) {
			j[name] = field;
		});
	}

	// Generic from_json for visitable structs in namespace AS (handles optional)
	template<typename T>
	typename std::enable_if<visit_struct::traits::is_visitable<T>::value>::type
		from_json(const nlohmann::json& j, T& value) {
		visit_struct::for_each(value, [&](const char* name, auto& field) {
			using FieldType = std::decay_t<decltype(field)>;
			if constexpr (is_optional<FieldType>::value) {
				if (auto it = j.find(name); it != j.end() && !it->is_null()) {
					field = it->template get<typename FieldType::value_type>();
				}
				else {
					field = std::nullopt;
				}
			}
			else {
				j.at(name).get_to(field);
			}
		});
	}
}

// Generic operator<< for variants
template<typename... Ts>
std::ostream& operator<<(std::ostream& os, const std::variant<Ts...>& var) {
	std::visit([&os](auto&& arg) {
		os << arg;
	}, var);
	return os;
}

// Overload operator<< for std::optional<T>
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::optional<T>& opt) {
	if (opt) {
		return os << *opt;
	}
	return os << "nullopt";
}

// Generic operator<< for visitable structs
template<typename T>
typename std::enable_if<visit_struct::traits::is_visitable<T>::value, std::ostream&>::type
operator<<(std::ostream& os, const T& value) {
	visit_struct::for_each(value, [&](const char* name, const auto& field) {
		os << name << ": " << field << '\n';
	});
	return os;
}

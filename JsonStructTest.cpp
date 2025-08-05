#include "my_guid.hpp"
#include "my_array.hpp"
#include "nlohmann/json.hpp"
#include "visit_struct/visit_struct.hpp"
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <type_traits>

template <typename T>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {}; 

namespace AS {
	struct my_type {
		my_guid id;
		int a;
		float b;
		std::string c;
		std::optional<bool> d;
		my_array<my_guid, 2> links;
	};

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

VISITABLE_STRUCT(AS::my_type, id, a, b, c, d, links); // register struct as visitable

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

int main()
{
	using namespace AS;
	auto original = my_type{ my_guid{}, 42, 3.14f, "Hello World", std::optional<bool>{}, my_array<my_guid, 2>{ my_guid{}, my_guid{} } };

	// Serialize
	nlohmann::json j = original;
	std::cout << "Serialized: " << j.dump(2) << '\n';

	// Deserialize
	auto deserialized = j.get<my_type>();
	std::cout << "Deserialized: " << deserialized;

	return 0;
}

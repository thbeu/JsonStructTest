#include "my_guid.hpp"
#include "nlohmann/json.hpp"
#include "visit_struct/visit_struct.hpp"
#include <iostream>
#include <ostream>
#include <string>
#include <type_traits>

namespace AS {
	struct my_type {
		my_guid id;
		int a;
		float b;
		std::string c;
	};

	// Generic to_json for visitable structs in namespace AS
	template<typename T>
	typename std::enable_if<visit_struct::traits::is_visitable<T>::value>::type
		to_json(nlohmann::json& j, const T& value) {
		visit_struct::for_each(value, [&](const char* name, const auto& field) {
			j[name] = field;
		});
	}

	// Generic from_json for visitable structs in namespace AS
	template<typename T>
	typename std::enable_if<visit_struct::traits::is_visitable<T>::value>::type
		from_json(const nlohmann::json& j, T& value) {
		visit_struct::for_each(value, [&](const char* name, auto& field) {
			j.at(name).get_to(field);
		});
	}
}

VISITABLE_STRUCT(AS::my_type, id, a, b, c); // register struct as visitable

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
	auto original = my_type{ my_guid{}, 42, 3.14f, "Hello World" };

	// Serialize
	nlohmann::json j = original;
	std::cout << "Serialized: " << j.dump(2) << '\n';

	// Deserialize
	auto deserialized = j.get<my_type>();
	std::cout << "Deserialized: " << deserialized;

	return 0;
}

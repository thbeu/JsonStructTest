#include "my_guid.hpp"
#include "my_array.hpp"
#include "boiler_plate.hpp"
#include "nlohmann/json.hpp"
#include "visit_struct/visit_struct.hpp"
#include <iostream>
#include <optional>
#include <string>
#include <variant>

namespace AS {
	struct my_type {
		my_guid id;
		int a;
		float b;
		std::string c;
		std::optional<bool> d;
		my_array<my_guid, 2> arr;
		std::variant<std::string, int> var;
	};
}

VISITABLE_STRUCT(AS::my_type, id, a, b, c, d, arr, var); // register struct as visitable

int main()
{
	using namespace AS;
	auto original = my_type{ my_guid{}, 42, 3.14f, "Hello World", std::optional<bool>{}, my_array<my_guid, 2>{ my_guid{}, my_guid{} }, "One" };

	// Serialize
	nlohmann::json j = original;
	std::cout << "Serialized: " << j.dump(2) << '\n';

	// Deserialize
	auto deserialized = j.get<my_type>();
	std::cout << "Deserialized: " << deserialized;

	return 0;
}

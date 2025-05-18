#include <cppjson/object.hpp>
#include <print>

int main()
{
	cppjson::Object object{};
	std::println("{}", object);
	object["test1"] = "Hello World";
	object["test2"] = 123.0;
	object["sub"]["veryNested"] = 6.0;

	std::println("{}", object);
	std::println("object[\"test1\"] = {}", object["test1"]);
	const std::string test = object["test1"];
	std::println("test = {}", test);
}

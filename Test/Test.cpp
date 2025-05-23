#include <cppjson/cppjson.hpp>
#include <print>

int main()
{
	cppjson::Object object{};
	std::println("{}", object);
	object["test1"] = "Hello World";
	object["test2"] = 123.0;
	object["sub"]["veryNested"] = 6.0;
	cppjson::Array& array = object["array"];
	array[] = 2;
	array[] = 6.0;
	array[0] = 1;
	array[] = "Stirng";
	array.EmplaceBack(nullptr);
	try
	{
		array[2] = true;
	}
	catch (const std::logic_error& error)
	{
		std::println("Error = {}", error.what());
	}

	std::println("{}", object);
	std::println("object[\"test1\"] = {}", object["test1"]);
	const std::string test = object["test1"];
	std::println("test = {}", test);
}

#include <cppjson/object.hpp>
#include <print>

int main()
{
	cppjson::Object object{};
	std::println("{}", object);
	object["test1"] = "Hello World";
	object["test2"] = 123.0;
	std::println("{}", object);
}

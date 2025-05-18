#include <cppjson/object.hpp>
#include <print>

int main()
{
	cppjson::Object object{};
	std::println("{}", object);
	object["test"] = "Hello World";
	std::println("{}", object);
}

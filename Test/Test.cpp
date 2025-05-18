#include <cppjson/object.hpp>
#include <print>

int main()
{
	cppjson::JsonObject object{};
	object.As<std::string>() = "Purr world!";
	std::println("{}", object);
}

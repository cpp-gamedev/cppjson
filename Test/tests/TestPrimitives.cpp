#include <cppjson/cppjson.hpp>
#include <print>

int main()
{
	cppjson::Object object{};
	object["test1"] = "Hello World";
	object["test2"] = 123.0;

	if ((std::string)object["test1"] != "Hello World" || (double)object["test2"] != 123.0)
	{
		std::println("TestPrimitives failed");
		return 1;
	}
	return 0;
}

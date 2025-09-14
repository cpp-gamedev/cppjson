#include <cppjson/cppjson.hpp>
#include <print>

int main()
{
	cppjson::Object object{};
	object["sub"]["veryNested"] = 6.0;

	if ((double)object["sub"]["veryNested"] != 6.0)
	{
		std::println("TestNestedObjects failed");
		return 1;
	}
	return 0;
}

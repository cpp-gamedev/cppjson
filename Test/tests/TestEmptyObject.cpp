#include <cppjson/cppjson.hpp>
#include <print>

int main()
{
	cppjson::Object object{};
	if (!object.IsEmpty())
	{
		std::println("TestEmptyObject failed");
		return 1;
	}
	return 0;
}

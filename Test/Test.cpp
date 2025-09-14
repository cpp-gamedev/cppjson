#include <gtest/gtest.h>
#include <cppjson/cppjson.hpp>

TEST(BasicTests, ArraySize)
{
	cppjson::Array array{};
	array[] = 1;
	array[] = 2;
	array[] = 3.0;
	EXPECT_TRUE(array.Size() == 3);
}

TEST(BasicTests, InvalidAssignment)
{
	cppjson::Object obj{};
	obj["number"] = 123.0;
	EXPECT_THROW({ obj["number"] = "NaN"; }, std::logic_error);
}

TEST(BasicTests, NestedObjects)
{
	cppjson::Object object{};
	object["sub"]["veryNested"] = 6.0;

	EXPECT_EQ(6.0, (double)object["sub"]["veryNested"]);
}

TEST(BasicTests, ObjectTypes)
{
	cppjson::Object obj{};
	obj["string"] = "Hello";
	obj["number"] = 42.0;
	obj["boolean"] = true;
	obj["null"] = nullptr;

	EXPECT_TRUE(IsType<std::string>(obj["string"]));
	EXPECT_TRUE(IsType<double>(obj["number"]));
	EXPECT_TRUE(IsType<bool>(obj["boolean"]));
	EXPECT_TRUE(IsType<std::nullptr_t>(obj["null"]));
}

TEST(BasicTests, Primitives)
{
	cppjson::Object object{};
	object["test1"] = "Hello World";
	object["test2"] = 123.0;

	EXPECT_EQ("Hello World", static_cast<const std::string&>(object["test1"]));
	EXPECT_EQ(123.0, (double)object["test2"]);
}

TEST(BasicTests, ValueComparisons)
{
	cppjson::Object obj{};
	obj["a"] = 5.0;
	obj["b"] = 5.0;
	obj["c"] = 10.0;

	EXPECT_TRUE(obj["a"] == obj["b"]);
	EXPECT_FALSE(obj["a"] == obj["c"]);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

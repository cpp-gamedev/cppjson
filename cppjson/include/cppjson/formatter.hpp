#include "object.hpp"
#include <format>

template <>
struct std::formatter<cppjson::JsonObject>
{
	constexpr auto parse(std::format_parse_context& context) { return context.begin(); }

	auto format(const cppjson::JsonObject& object, std::format_context& context) const
	{
		switch (object._dataType)
		{
		case cppjson::JsonType::Null: return std::format_to(context.out(), "null");
		case cppjson::JsonType::Bool: return std::format_to(context.out(), "{}", object.DangerousAs<bool>());
		case cppjson::JsonType::Number: return std::format_to(context.out(), "{}", object.DangerousAs<double>());
		case cppjson::JsonType::String: return std::format_to(context.out(), "\"{}\"", object.DangerousAs<std::string>());
		case cppjson::JsonType::Object:
		{
			const auto& node = object.DangerousAs<cppjson::Object>();

			std::string built = "{ ";
			for (const auto& [key, value] : node._nodes) built += std::format("\"{}\": {}, ", key, value);

			if (!node._nodes.empty()) // remove trailing commas
			{
				built.pop_back();
				built.pop_back();
				built += " }";
			}
			else
				built += "}";

			return std::format_to(context.out(), "{}", built);
		}
		case cppjson::JsonType::Array:
		{
			const auto& array = object.DangerousAs<cppjson::Array>();

			std::string built = "[ ";
			for (const auto& element : array._objects) built += std::format("{}, ", element);

			if (!array._objects.empty()) // remove trailing commas
			{
				built.pop_back();
				built.pop_back();
				built += " ]";
			}
			else
				built += "]";

			return std::format_to(context.out(), "{}", built);
		}
		}

		throw std::logic_error("Unknown type");
	}
};

template <>
struct std::formatter<cppjson::Object>
{
	constexpr auto parse(std::format_parse_context& context) { return context.begin(); }

	auto format(const cppjson::Object& object, std::format_context& context) const
	{
		std::string built = "{ ";
		for (const auto& [key, value] : object._nodes) built += std::format("\"{}\": {}, ", key, value);

		if (!object._nodes.empty()) // remove trailing commas
		{
			built.pop_back();
			built.pop_back();
			built += " }";
		}
		else
			built += "}";

		return std::format_to(context.out(), "{}", built);
	}
};

template <>
struct std::formatter<cppjson::Array>
{
	constexpr auto parse(std::format_parse_context& context) { return context.begin(); }

	auto format(const cppjson::Array& array, std::format_context& context) const
	{
		std::string built = "[ ";
		for (const auto& element : array._objects) built += std::format("{}, ", element);

		if (!array._objects.empty()) // remove trailing commas
		{
			built.pop_back();
			built.pop_back();
			built += " ]";
		}
		else
			built += "]";

		return std::format_to(context.out(), "{}", built);
	}
};

template <>
struct std::formatter<cppjson::Object::ObjectProxy>
{
	constexpr auto parse(std::format_parse_context& context) { return context.begin(); }

	auto format(const cppjson::Object::ObjectProxy& object, std::format_context& context) const
	{
		return std::format_to(context.out(), "{}", object._object.get());
	}
};

template <>
struct std::formatter<cppjson::Object::ConstObjectProxy>
{
	constexpr auto parse(std::format_parse_context& context) { return context.begin(); }

	auto format(const cppjson::Object::ConstObjectProxy& object, std::format_context& context) const
	{
		return std::format_to(context.out(), "{}", object._object.get());
	}
};

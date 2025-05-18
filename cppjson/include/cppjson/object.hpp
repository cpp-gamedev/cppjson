#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <string>
#include <cstddef>
#include <format>

namespace cppjson
{
	enum struct JsonType
	{
		Null,
		String,
		Object,
		Number,
		Bool,
		// TODO: Array
	};

	class JsonObject
	{
	public:
		explicit JsonObject();
		~JsonObject();

		template <typename T>
		T& As() noexcept(false);

		template <typename T>
		const T& As() const noexcept(false);
	private:
		JsonType _dataType{};
		std::byte* _dataStorage{};

		void Destroy();
		template <typename T>
		T& DangerousAs() noexcept { return *std::launder(reinterpret_cast<T*>(this->_dataStorage)); }
		template <typename T>
		const T& DangerousAs() const noexcept { return *std::launder(reinterpret_cast<T*>(this->_dataStorage)); }

		friend struct std::formatter<cppjson::JsonObject>;
	};
}

template<>
struct std::formatter<cppjson::JsonObject>
{
	constexpr auto parse(std::format_parse_context& context)
	{
		return context.begin();
	}

	auto format(const cppjson::JsonObject& object, std::format_context& context) const
	{
		switch (object._dataType)
		{
		case cppjson::JsonType::Null: return std::format_to(context.out(), "null");
		case cppjson::JsonType::Bool: return std::format_to(context.out(), "{}", object.DangerousAs<bool>());
		case cppjson::JsonType::Number: return std::format_to(context.out(), "{}", object.DangerousAs<double>());
		case cppjson::JsonType::String: return std::format_to(context.out(), "{}", object.DangerousAs<std::string>());
		}

		throw std::logic_error("Unknown type");
	}
};

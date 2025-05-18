#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <string>
#include <cstddef>
#include <format>
#include <unordered_map>

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

	class Object
	{
	public:
		explicit Object() = default;
		Object(const Object&) = default;
		Object(Object&&) = default;	  
		Object& operator=(const Object&) = default;
		Object& operator=(Object&&) = default;
		~Object() = default;

		template <typename T>
		T& operator[](const std::string& key)
		{
			return this->_nodes[key].As<T>();
		}					   
		template <typename T>
		const T& operator[](const std::string& key) const
		{
			if (!this->_nodes.contains(key)) throw std::logic_error("Invalid key" + key);
			return this->_nodes.at(key).As<T>();
		}
	private:
		std::unordered_map<std::string, JsonObject> _nodes{};
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

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <format>
#include <string>
#include <unordered_map>
#include <functional>
#include <concepts>

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
        JsonObject(const JsonObject& other);
        JsonObject(JsonObject&& other);
        JsonObject& operator=(const JsonObject& other);
        JsonObject& operator=(JsonObject&& other);
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
		T& DangerousAs() noexcept
		{
			return *std::launder(reinterpret_cast<T*>(this->_dataStorage));
		}
		template <typename T>
		const T& DangerousAs() const noexcept
		{
			return *std::launder(reinterpret_cast<T*>(this->_dataStorage));
		}

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

		class ObjectProxy
		{
		public:
			explicit ObjectProxy(JsonObject& object) : _object(std::ref(object)) {}

			template <typename T>
				requires (!std::same_as<std::remove_cvref_t<T>, JsonObject>)
			explicit(false) operator T&()
			{
				return this->_object.get().As<T>();
			}

			template <typename T>
				requires (!std::same_as<std::remove_cvref_t<T>, JsonObject>)
			explicit(false) operator const T&() const
			{
				return this->_object.get().As<T>();
			}

			template <typename T>
			T& operator=(T&& assignment)
			{
				return static_cast<T&>(*this) = std::forward<T>(assignment);
			}

			template <std::size_t N>
			std::string& operator=(const char(&str)[N])
			{
				return static_cast<std::string&>(*this) = std::string{ str };
			}
 
			ObjectProxy operator[](const std::string& key);
            tempalte <std::size_t N>
            ObjectProxy operator[](const char(&key)[N]) { return (*this)[std::string{key}]; }
		private:
			std::reference_wrapper<JsonObject> _object;

			friend struct std::formatter<cppjson::Object::ObjectProxy>;
		};


		class ConstObjectProxy
		{
		public:
			explicit ConstObjectProxy(const JsonObject& object) : _object(std::ref(object)) {}
			template <typename T>
			explicit(false) operator const T&() const
			{
				return this->_object.get().As<T>();
			}

			ConstObjectProxy operator[](const std::string& key) const;
		private:
			std::reference_wrapper<const JsonObject> _object;

			friend struct std::formatter<cppjson::Object::ConstObjectProxy>;
		};

		ObjectProxy operator[](const std::string& key)
		{
			return ObjectProxy{ this->_nodes[key] };
		}

		ConstObjectProxy operator[](const std::string& key) const
		{
			if (!this->_nodes.contains(key)) throw std::logic_error("Invalid key" + key);

			return ConstObjectProxy{ this->_nodes.at(key) };
		}

	  private:
		std::unordered_map<std::string, JsonObject> _nodes{};

		friend struct std::formatter<cppjson::JsonObject>;
		friend struct std::formatter<cppjson::Object>;
	};
} // namespace cppjson


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
			for (const auto& [key, value] : node._nodes)
				built += std::format("\"{}\": {}, ", key, value);

			if (!node._nodes.empty()) // remove trailing commas
			{
				built.pop_back();
				built.pop_back();
				built += " }";
			}
			else built += "}";

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
		for (const auto& [key, value] : object._nodes)
			built += std::format("\"{}\": {}, ", key, value);
		
		if (!object._nodes.empty()) // remove trailing commas
		{
			built.pop_back();
			built.pop_back();
			built += " }";
		}
		else built += "}";

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

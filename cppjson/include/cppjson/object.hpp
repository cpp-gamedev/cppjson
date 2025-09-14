#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <format>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace cppjson
{
	enum struct JsonType
	{
		Null,
		String,
		Object,
		Number,
		Bool,
		Array
	};

	class JsonObject
	{
	  public:
		explicit JsonObject();
		JsonObject(const JsonObject& other);
		JsonObject(JsonObject&& other) noexcept;
		JsonObject& operator=(const JsonObject& other);
		JsonObject& operator=(JsonObject&& other) noexcept;
		~JsonObject();

		template <typename T>
		T& As() noexcept(false);

		template <typename T>
		const T& As() const noexcept(false);

		[[nodiscard]] bool operator==(const JsonObject& other) const;

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

		template <typename T>
		friend bool IsType(const JsonObject& object) noexcept;
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

		[[nodiscard]] bool IsEmpty() const noexcept { return this->_nodes.empty(); }

		[[nodiscard]] bool operator==(const Object& other) const;

		class ObjectProxy
		{
		  public:
			explicit ObjectProxy(JsonObject& object) : _object(std::ref(object)) {}

			template <typename T>
				requires(!std::same_as<std::remove_cvref_t<T>, JsonObject>)
			explicit(false) operator T&()
			{
				return this->_object.get().As<std::remove_cvref_t<T>>();
			}

			template <typename T>
				requires(!std::same_as<std::remove_cvref_t<T>, JsonObject>)
			explicit(false) operator const T&() const
			{
				return this->_object.get().As<const std::remove_cvref_t<T>>();
			}

			template <typename T>
			std::conditional_t<std::integral<T> && !std::same_as<T, bool>, void, T&> operator=(T&& assignment)
			{
				if constexpr (std::integral<T> && !std::same_as<T, bool>) static_cast<double&>(*this) = static_cast<double>(assignment);
				else
					return static_cast<T&>(*this) = std::forward<T>(assignment);
			}

			template <std::size_t N>
			std::string& operator=(const char (&str)[N])
			{
				return static_cast<std::string&>(*this) = std::string{str};
			}

			ObjectProxy operator[](const std::string& key);
			template <std::size_t N>
			ObjectProxy operator[](const char (&key)[N])
			{
				return (*this)[std::string{key}];
			}
			[[nodiscard]] bool operator==(const ObjectProxy& other) const { return this->_object.get() == other._object.get(); }

		  private:
			std::reference_wrapper<JsonObject> _object;

			friend struct std::formatter<cppjson::Object::ObjectProxy>;
			template <typename T>
			friend bool IsType(const Object::ObjectProxy& proxy) noexcept;
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

		ObjectProxy operator[](const std::string& key) { return ObjectProxy{this->_nodes[key]}; }

		ConstObjectProxy operator[](const std::string& key) const
		{
			if (!this->_nodes.contains(key)) throw std::logic_error("Invalid key" + key);

			return ConstObjectProxy{this->_nodes.at(key)};
		}

	  private:
		std::unordered_map<std::string, JsonObject> _nodes{};

		friend struct std::formatter<cppjson::JsonObject>;
		friend struct std::formatter<cppjson::Object>;
	};

	class Array
	{
	  public:
		explicit Array() = default;
		~Array() = default;

		Object::ObjectProxy operator[]() { return Object::ObjectProxy{this->_objects.emplace_back()}; }

		Object::ObjectProxy EmplaceBack(const auto& object)
		{
			if constexpr (std::same_as<decltype(object), std::nullptr_t>) return Object::ObjectProxy{this->_objects.emplace_back()};
			else
			{
				auto& emplaced = this->_objects.emplace_back();
				emplaced.As<std::remove_cvref_t<decltype(object)>>() = object;
				return Object::ObjectProxy{emplaced};
			}
		}

		Object::ObjectProxy operator[](const std::size_t index)
		{
			if (index >= this->_objects.size()) throw std::logic_error("Out of bound");
			return Object::ObjectProxy{this->_objects.at(index)};
		}

		Object::ConstObjectProxy operator[](const std::size_t index) const
		{
			if (index >= this->_objects.size()) throw std::logic_error("Out of bound");
			return Object::ConstObjectProxy{this->_objects.at(index)};
		}

		[[nodiscard]] std::size_t Size() const noexcept { return this->_objects.size(); }

		[[nodiscard]] bool operator==(const Array& other) const
		{
			if (this->_objects.size() != other._objects.size()) return false;
			return std::equal(this->_objects.begin(), this->_objects.end(), other._objects.begin());
		}

	  private:
		std::vector<JsonObject> _objects{};

		friend struct std::formatter<cppjson::JsonObject>;
		friend struct std::formatter<cppjson::Array>;
	};

	template <typename T>
	[[nodiscard]] bool IsType(const JsonObject& object) noexcept
	{
		if constexpr (std::same_as<std::remove_cvref_t<T>, std::nullptr_t>) return object._dataType == JsonType::Null;
		else if constexpr (std::same_as<std::remove_cvref_t<T>, std::string>) return object._dataType == JsonType::String;
		else if constexpr (std::same_as<std::remove_cvref_t<T>, Object>) return object._dataType == JsonType::Object;
		else if constexpr (std::same_as<std::remove_cvref_t<T>, double>) return object._dataType == JsonType::Number;
		else if constexpr (std::same_as<std::remove_cvref_t<T>, bool>) return object._dataType == JsonType::Bool;
		else if constexpr (std::same_as<std::remove_cvref_t<T>, Array>) return object._dataType == JsonType::Array;
		else
			return false;
	}

	template <typename T>
	[[nodiscard]] bool IsType(const Object::ObjectProxy& proxy) noexcept
	{
		return IsType<T>(proxy._object.get());
	}
} // namespace cppjson

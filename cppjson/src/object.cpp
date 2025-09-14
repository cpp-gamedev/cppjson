#include "cppjson/object.hpp"
#include <cstring>
#include <new>
#include <stdexcept>
#include <utility>

constexpr std::size_t DataStorageSize = std::max({sizeof(std::string), sizeof(cppjson::Object), sizeof(double), sizeof(bool)});

cppjson::JsonObject::JsonObject() : _dataStorage(static_cast<std::byte*>(::operator new(DataStorageSize))) {}

cppjson::JsonObject::JsonObject(const cppjson::JsonObject& other)
{
	if (other._dataStorage == nullptr) return;

	this->_dataType = other._dataType;
	this->_dataStorage = static_cast<std::byte*>(::operator new(DataStorageSize));
	std::memcpy(this->_dataStorage, other._dataStorage, DataStorageSize);
}
cppjson::JsonObject::JsonObject(JsonObject&& other) noexcept
{
	this->_dataType = std::exchange(other._dataType, cppjson::JsonType::Null);
	this->_dataStorage = std::exchange(other._dataStorage, static_cast<std::byte*>(::operator new(DataStorageSize)));
}
cppjson::JsonObject& cppjson::JsonObject::operator=(const cppjson::JsonObject& other)
{
	if (&other != this)
	{
		this->_dataType = other._dataType;
		this->_dataStorage = static_cast<std::byte*>(::operator new(DataStorageSize));
		std::memcpy(this->_dataStorage, other._dataStorage, DataStorageSize);
	}
	return *this;
}
cppjson::JsonObject& cppjson::JsonObject::operator=(cppjson::JsonObject&& other) noexcept
{
	if (&other != this)
	{
		this->_dataType = std::exchange(other._dataType, cppjson::JsonType::Null);
		this->_dataStorage = std::exchange(other._dataStorage, static_cast<std::byte*>(::operator new(DataStorageSize)));
	}
	return *this;
}
cppjson::JsonObject::~JsonObject()
{
	this->Destroy();
	::operator delete(this->_dataStorage);
}

bool cppjson::JsonObject::operator==(const JsonObject& other) const
{
	if (other._dataType != this->_dataType) return false;
	switch (this->_dataType)
	{
	case JsonType::Null: return true;
	case JsonType::Number: return this->DangerousAs<double>() == other.DangerousAs<double>();
	case JsonType::Bool: return this->DangerousAs<bool>() == other.DangerousAs<bool>();
	case JsonType::String: return this->DangerousAs<std::string>() == other.DangerousAs<std::string>();
	case JsonType::Object: return this->DangerousAs<cppjson::Object>() == other.DangerousAs<cppjson::Object>();
	case JsonType::Array: return this->DangerousAs<cppjson::Array>() == other.DangerousAs<cppjson::Array>();
	default: return false;
	}
}

void cppjson::JsonObject::Destroy(void)
{
	using cppjson::Array;
	using cppjson::Object;
	using std::string;

	switch (std::exchange(this->_dataType, JsonType::Null))
	{
	case JsonType::Null:
	case JsonType::Number:
	case JsonType::Bool: break;
	case JsonType::String: return DangerousAs<std::string>().~string();
	case JsonType::Object: return DangerousAs<cppjson::Object>().~Object();
	case JsonType::Array: return DangerousAs<cppjson::Array>().~Array();
	}
}

template <>
std::string& cppjson::JsonObject::As<std::string>() noexcept(false)
{
	if (this->_dataType == JsonType::Null)
	{
		this->_dataType = JsonType::String;
		return *new (this->_dataStorage) std::string{};
	}

	if (this->_dataType != JsonType::String) throw std::logic_error("Cannot convert this object to a string");
	return DangerousAs<std::string>();
}

template <>
double& cppjson::JsonObject::As<double>() noexcept(false)
{
	if (this->_dataType == JsonType::Null)
	{
		this->_dataType = JsonType::Number;
		return *new (this->_dataStorage) double{};
	}

	if (this->_dataType != JsonType::Number) throw std::logic_error("Cannot convert this object to a double");
	return DangerousAs<double>();
}

template <>
bool& cppjson::JsonObject::As<bool>() noexcept(false)
{
	if (this->_dataType == JsonType::Null)
	{
		this->_dataType = JsonType::Bool;
		return *new (this->_dataStorage) bool{};
	}

	if (this->_dataType != JsonType::Bool) throw std::logic_error("Cannot convert this object to a bool");
	return DangerousAs<bool>();
}

template <>
cppjson::Object& cppjson::JsonObject::As<cppjson::Object>() noexcept(false)
{
	if (this->_dataType == JsonType::Null)
	{
		this->_dataType = JsonType::Object;
		return *new (this->_dataStorage) cppjson::Object{};
	}

	if (this->_dataType != JsonType::Object) throw std::logic_error("Cannot convert this object to a bool");
	return DangerousAs<cppjson::Object>();
}

template <>
cppjson::Array& cppjson::JsonObject::As<cppjson::Array>() noexcept(false)
{
	if (this->_dataType == JsonType::Null)
	{
		this->_dataType = JsonType::Array;
		return *new (this->_dataStorage) cppjson::Array{};
	}

	if (this->_dataType != JsonType::Array) throw std::logic_error("Cannot convert this object to an array");
	return DangerousAs<cppjson::Array>();
}

template <>
std::nullptr_t& cppjson::JsonObject::As<std::nullptr_t>() noexcept(false)
{
	if (std::exchange(this->_dataType, JsonType::Null) == JsonType::Null) return DangerousAs<std::nullptr_t>();

	Destroy();
	return *new (this->_dataStorage) std::nullptr_t{};
}

template <>
const std::string& cppjson::JsonObject::As<std::string>() const noexcept(false)
{
	if (this->_dataType != JsonType::String) throw std::logic_error("Cannot convert this object to a string");
	return DangerousAs<std::string>();
}

template <>
const double& cppjson::JsonObject::As<double>() const noexcept(false)
{
	if (this->_dataType != JsonType::Number) throw std::logic_error("Cannot convert this object to a double");
	return DangerousAs<double>();
}

template <>
const bool& cppjson::JsonObject::As<bool>() const noexcept(false)
{
	if (this->_dataType != JsonType::Bool) throw std::logic_error("Cannot convert this object to a bool");
	return DangerousAs<bool>();
}

template <>
const cppjson::Object& cppjson::JsonObject::As<cppjson::Object>() const noexcept(false)
{
	if (this->_dataType != JsonType::Object) throw std::logic_error("Cannot convert this object to an object");
	return DangerousAs<cppjson::Object>();
}

template <>
const cppjson::Array& cppjson::JsonObject::As<cppjson::Array>() const noexcept(false)
{
	if (this->_dataType != JsonType::Array) throw std::logic_error("Cannot convert this object to an Array");
	return DangerousAs<cppjson::Array>();
}

template <>
const std::nullptr_t& cppjson::JsonObject::As<std::nullptr_t>() const noexcept(false)
{
	if (this->_dataType != JsonType::Null) throw std::logic_error("Cannot convert this object to a null");
	return DangerousAs<std::nullptr_t>();
}

cppjson::Object::ObjectProxy cppjson::Object::ObjectProxy::operator[](const std::string& key) { return ObjectProxy{this->_object.get().As<Object>()[key]}; }

cppjson::Object::ConstObjectProxy cppjson::Object::ConstObjectProxy::operator[](const std::string& key) const
{
	return ConstObjectProxy{this->_object.get().As<Object>()[key]};
}

bool cppjson::Object::operator==(const Object& other) const
{
	if (this->_nodes.size() != other._nodes.size()) return false;
	for (const auto& [key, value] : this->_nodes)
	{
		if (!other._nodes.contains(key)) return false;
		if (!(value == other._nodes.at(key))) return false;
	}
	return true;
}

#include "cppjson/object.hpp"
#include <new>
#include <stdexcept>

constexpr std::size_t DataStorageSize = std::max({sizeof(std::string), sizeof(cppjson::Object), sizeof(double), sizeof(bool)});

cppjson::JsonObject::JsonObject() : _dataStorage(static_cast<std::byte*>(::operator new(DataStorageSize))) {}

cppjson::JsonObject::~JsonObject()
{
	this->Destroy();
	::operator delete(this->_dataStorage);
}

void cppjson::JsonObject::Destroy(void)
{
	using std::string;

	switch (std::exchange(this->_dataType, JsonType::Null))
	{
	case JsonType::Null:
	case JsonType::Number:
	case JsonType::Bool: break;
	case JsonType::String: DangerousAs<std::string>().~string();
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
std::nullptr_t& cppjson::JsonObject::As<std::nullptr_t>() noexcept(false)
{
	if (std::exchange(this->_dataType, JsonType::Null) == JsonType::Null) return DangerousAs<std::nullptr_t>();

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
const std::nullptr_t& cppjson::JsonObject::As<std::nullptr_t>() const noexcept(false)
{
	if (this->_dataType != JsonType::Null) throw std::logic_error("Cannot convert this object to a null");
	return DangerousAs<std::nullptr_t>();
}

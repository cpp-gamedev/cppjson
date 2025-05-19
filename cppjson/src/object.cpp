#include "cppjson/object.hpp"
#include <new>
#include <stdexcept>
#include <cstdlib>

constexpr std::size_t DataStorageSize = std::max({sizeof(std::string), sizeof(cppjson::Object), sizeof(double), sizeof(bool)});

cppjson::JsonObject::JsonObject() : _dataStorage(static_cast<std::byte*>(::operator new(DataStorageSize))) {}

cppjson::JsonObject::JsonObject(const cppjson::JsonObject& other)
{
	if (other._dataStorage == nullptr) return;
	
	this->_dataType = other._dataType;
	this->_dataStorage = static_cast<std::byte*>(::operator new(DataStorageSize));
	std::memcpy(this->_dataStorage, other._dataStorage, DataStorageSize);
}
cppjson::JsonObject::JsonObject(JsonObject&& other)
{
     this->_dataType = std::exchange(other._dataType, cppjson::JsonType::Null);
     this->_dataStorage = std::exchange(other._dataStorage, ::operator new(DataStorageSize));
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
cppjson::JsonObject& cppjson::JsonObject::operator=(cppjson::JsonObject&& other)
{
    if (&other != this)
    {
		this->_dataType = std::exchange(other._dataType, cppjson::JsonType::Null);
		this->_dataStorage = std::exchange(other._dataStorage, ::operator new(DataStorageSize));
	}
    return *this;
}
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

cppjson::Object::ObjectProxy cppjson::Object::ObjectProxy::operator[](const std::string& key)
{
	return ObjectProxy{ this->_object.get().As<Object>()[key] };
}


cppjson::Object::ConstObjectProxy cppjson::Object::ConstObjectProxy::operator[](const std::string& key) const
{
	return ConstObjectProxy{ this->_object.get().As<Object>()[key] };
}

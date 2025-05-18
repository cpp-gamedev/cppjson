#pragma once

#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <string>
#include <cstddef>

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
	};
}

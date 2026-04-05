#pragma once

#include <vector>
#include <cstdint>

namespace NC {
	class Writer {
		std::vector<uint8_t> _buffer;
	public:
		template<typename T>
		void write_integer(T value);

		void write_data(void* data, size_t lenght);

		const std::vector<uint8_t>& get_buffer() { return this->_buffer; }
	};
}
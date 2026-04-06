#pragma once

#include <vector>
#include <cstdint>

namespace NC {
	class Writer {
		std::vector<uint8_t> _buffer;
	public:
		template<typename T>
		void write_integer(T value);

		void write_data(void* data, int lenght);

		template<typename T>
		void write_data(T data, int lenght) {
			write_data(data, sizeof(T) * lenght);
		}

		const std::vector<uint8_t>& get_buffer() { return this->_buffer; }
	};
}
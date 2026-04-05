#pragma once

#include <vector>
#include <cstdint>

namespace NC {
	class Reader {
		std::vector<uint8_t> _buffer;
		uint64_t _cursor = 0;
	public:
		Reader(const std::vector<uint8_t>& buffer) : _buffer(buffer) {}

		template<typename T>
		T read_intager();

		template<typename T>
		T read_data(size_t lenght);

		const std::vector<uint8_t>& get_buffer() { return this->_buffer; }
	};
}
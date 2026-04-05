#include "NetworkCommon/Reader.h"
#include <stdexcept>

namespace NC {
	template<typename T>
	T Reader::read_intager() {
        static_assert(std::is_integral_v<T>, "Must be integer");

        T value = 0;

        for (size_t i = 0; i < sizeof(T); ++i) {
            value |= (T)this->_buffer[this->_cursor++] << (i * 8);
        }

        return value;
	}

    template<typename T>
    T Reader::read_data(size_t lenght) {
        if (this->_cursor + lenght > this->_buffer.size())
            throw std::runtime_error("Out of bounds");


        T value;
        std::memcpy(&value, &this->_buffer[this->_cursor], sizeof(T));
        this->_cursor += sizeof(T);

        return value;
    }
}
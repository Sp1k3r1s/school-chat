#include "NetworkCommon/Writer.h"

namespace NC {
    template<typename T>
    void Writer::write_integer(T value) {
        static_assert(std::is_integral_v<T>, "Must be integer");

        for (size_t i = 0; i < sizeof(T); ++i) {
            _buffer.push_back((value >> (i * 8)) & 0xFF);
        }
    }

    void Writer::write_data(void* data, size_t lenght) {
        const uint8_t* ptr = (const uint8_t*)(data);
        this->_buffer.insert(this->_buffer.end(), ptr, ptr + lenght);
    }
}
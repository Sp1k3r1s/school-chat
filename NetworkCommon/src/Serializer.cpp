#include "NetworkCommon/Serializer.h"

namespace NC {
	void write_auth_packet(Writer& writer, AuthPacket p) {
		writer.write_data<char*>(p.username, 64);
		writer.write_data<char*>(p.password, 256);
	}

	void write_message_packet(Writer& writer, MessagePacket p) {
		writer.write_integer<uint32_t>(p.message_len);
		writer.write_data<char*>(p.message, p.message_len);
	}
}
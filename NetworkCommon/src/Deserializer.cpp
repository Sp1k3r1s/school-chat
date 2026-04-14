#include "NetworkCommon/Deserializer.h"

namespace NC {
	AuthPacket read_auth_packet(const Reader& reader) {
		if (reader.read_intager<uint8_t>() == PACKET_AUTH) {

		}
	}

	MessagePacket read_message_packet(const Reader& reader) {

	}
}
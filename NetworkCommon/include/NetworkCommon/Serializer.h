#pragma once

#include <NetworkCommon/Writer.h>
#include <NetworkCommon/Packets.h>

namespace NC {
	void write_auth_packet(Writer& writer, AuthPacket p);
	void write_message_packet(Writer& writer, MessagePacket p);
}

// todo
// text serialization
// improve read write
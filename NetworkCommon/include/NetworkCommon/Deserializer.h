#pragma once

#include <NetworkCommon/Reader.h>
#include <NetworkCommon/Packets.h>

namespace NC {
	AuthPacket read_auth_packet(const Reader& reader);
	MessagePacket read_message_packet(const Reader& reader);
}
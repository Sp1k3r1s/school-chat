#pragma once
#include <string>

// todo
// struct auth packet
// struct get session role packet
// struct send message packet

enum Packets {
	PACKET_AUTH = 0x01,
	PACKET_MESSAGE = 0x02
};

typedef struct {
	char username[64];
	char password[256];
} AuthPacket;

typedef struct {
	uint32_t message_len;
	char* message;
} MessagePacket;
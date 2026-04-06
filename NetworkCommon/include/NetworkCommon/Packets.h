#pragma once
#include <string>

// todo
// struct auth packet
// struct get session role packet
// struct send message packet

typedef struct {
	char username[64];
	char password[256];
} AuthPacket;

typedef struct {
	uint32_t message_len;
	char* message;
} MessagePacket;
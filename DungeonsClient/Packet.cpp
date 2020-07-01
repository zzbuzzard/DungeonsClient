#include "pch.h"
#include "Packet.h"

Packet getPacketType(const void *data) {
	return *((Packet*)data);
}
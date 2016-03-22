/* Copyright: (c) Kayne Ruse 2013-2016
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 
 * 3. This notice may not be removed or altered from any source
 * distribution.
*/
#include "text_packet.hpp"

#include "serial_utility.hpp"

void serializeText(void* buffer, TextPacket* packet) {
	serialCopy(&buffer, &packet->type, sizeof(SerialPacketType));

	//content
	serialCopy(&buffer, packet->name, PACKET_STRING_SIZE);
	serialCopy(&buffer, packet->text, PACKET_STRING_SIZE);

	//location
	serialCopy(&buffer, &packet->roomIndex, sizeof(int));
	serialCopy(&buffer, &packet->origin.x, sizeof(double));
	serialCopy(&buffer, &packet->origin.y, sizeof(double));
	serialCopy(&buffer, &packet->range, sizeof(int));
}

void deserializeText(void* buffer, TextPacket* packet) {
	deserialCopy(&buffer, &packet->type, sizeof(SerialPacketType));

	//content
	deserialCopy(&buffer, packet->name, PACKET_STRING_SIZE);
	deserialCopy(&buffer, packet->text, PACKET_STRING_SIZE);

	//location
	deserialCopy(&buffer, &packet->roomIndex, sizeof(int));
	deserialCopy(&buffer, &packet->origin.x, sizeof(double));
	deserialCopy(&buffer, &packet->origin.y, sizeof(double));
	deserialCopy(&buffer, &packet->range, sizeof(int));
}
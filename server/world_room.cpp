/* Copyright: (c) Kayne Ruse 2013
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
#include "world_room.hpp"

#include <iostream>
#include <stdexcept>

using namespace std;

int worldRoomThread(void* ptr) {
	WorldRoom* room = reinterpret_cast<WorldRoom*>(ptr);
	try {
		room->Init();
		room->Loop();
		room->Quit();
	}
	catch(std::exception& e) {
		cerr << "Fatal Room Error: " << e.what() << endl;
		return 1;
	}
	return 0;
}

WorldRoom::WorldRoom() {
	//
}

WorldRoom::~WorldRoom() {
	//
}

void WorldRoom::Init() {
	//
}

void WorldRoom::Loop() {
	//
}

void WorldRoom::Quit() {
	//
}

void WorldRoom::HandlePacket(NetworkPacket packet) {
	switch(packet.meta.type) {
		case NetworkPacket::Type::PING:
			//NOT USED
		break;
		case NetworkPacket::Type::PONG:
			//NOT USED
		break;
//		case NetworkPacket::Type::BROADCAST_REQUEST:
//			//
//		break;
//		case NetworkPacket::Type::BROADCAST_RESPONSE:
//			//
//		break;
//		case NetworkPacket::Type::JOIN_REQUEST:
//			//
//		break;
//		case NetworkPacket::Type::JOIN_RESPONSE:
//			//
//		break;
//		case NetworkPacket::Type::DISCONNECT:
//			//
//		break;
//		case NetworkPacket::Type::SYNCHRONIZE:
//			//
//		break;

		//handle errors
		case NetworkPacket::Type::NONE:
			throw(runtime_error("NetworkPacket::Type::NONE encountered"));
		break;
		default:
			throw(runtime_error("Unknown NetworkPacket::Type encountered"));
		break;
	}
}
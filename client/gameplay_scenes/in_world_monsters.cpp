/* Copyright: (c) Kayne Ruse 2013-2015
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
#include "in_world.hpp"

#include "channels.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

//-------------------------
//monster management
//-------------------------

void InWorld::HandleMonsterCreate(MonsterPacket* const argPacket) {
	//check for logic errors
	if (monsterMap.find(argPacket->monsterIndex) != monsterMap.end()) {
		std::ostringstream msg;
		msg << "Double monster creation event; ";
		msg << "Index: " << argPacket->monsterIndex << "; ";
		msg << "Handle: " << argPacket->handle;
		throw(std::runtime_error(msg.str()));
	}

	//ignore monsters from other rooms
	if (roomIndex != argPacket->roomIndex) {
		//temporary error checking
		std::ostringstream msg;
		msg << "Monster from the wrong room received: ";
		msg << "monsterIndex: " << argPacket->monsterIndex << ", roomIndex: " << argPacket->roomIndex;
		throw(std::runtime_error(msg.str()));
	}

	//implicitly create the element
	BaseMonster* monster = &monsterMap[argPacket->monsterIndex];

	//fill the monster's info
	monster->SetHandle(argPacket->handle);
	monster->SetAvatar(argPacket->avatar);
	monster->SetBounds(argPacket->bounds);
	monster->SetOrigin(argPacket->origin);
	monster->SetMotion(argPacket->motion);

	//debug
	std::cout << "Monster Create, total: " << monsterMap.size() << std::endl;
}

void InWorld::HandleMonsterDelete(MonsterPacket* const argPacket) {
	//ignore if this monster doesn't exist
	std::map<int, BaseMonster>::iterator monsterIt = monsterMap.find(argPacket->monsterIndex);
	if (monsterIt == monsterMap.end()) {
		return;
	}


	//remove this monster
	monsterMap.erase(monsterIt);

	//debug
	std::cout << "Monster Delete, total: " << monsterMap.size() << std::endl;
}

void InWorld::HandleMonsterQueryExists(MonsterPacket* const argPacket) {
	//ignore monsters in a different room (sub-optimal)
	if (argPacket->roomIndex != roomIndex) {
		return;
	}

	//implicitly create the element
	BaseMonster* monster = &monsterMap[argPacket->monsterIndex];

	//fill the monster's info
	monster->SetHandle(argPacket->handle);
	monster->SetAvatar(argPacket->avatar);
	monster->SetBounds(argPacket->bounds);
	monster->SetOrigin(argPacket->origin);
	monster->SetMotion(argPacket->motion);

	//debug
	std::cout << "Monster Query, total: " << monsterMap.size() << std::endl;
}

void InWorld::HandleMonsterMovement(MonsterPacket* const argPacket) {
	//TODO: HandleMonsterMovement
}

void InWorld::HandleMonsterAttack(MonsterPacket* const argPacket) {
	//TODO: HandleMonsterAttack
}
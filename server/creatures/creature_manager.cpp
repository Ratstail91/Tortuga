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
#include "creature_manager.hpp"

#include "lua_utilities.hpp"

CreatureManager::CreatureManager() {
	//EMPTY
}

CreatureManager::~CreatureManager() {
	UnloadAll();
}

//arg: a list of creatures to be updated in the clients
void CreatureManager::Update(std::list<std::pair<const int, CreatureData*>>* creatureList, bool updateAll) {
	int ret;
	for (auto& it : elementMap) {
		ret = it.second.Update(lua);
		if (ret || updateAll) {
			creatureList->push_back(std::pair<const int, CreatureData*>(it.first, &it.second));
		}
	}
}

int CreatureManager::Create(std::string avatar, int scriptRef) {
	//implicitly create the new object
	elementMap.emplace( std::pair<int, CreatureData>(counter, CreatureData(avatar, scriptRef)) );

	runHook(lua, createRef, &elementMap.find(counter)->second, counter);

	//TODO: do various things like saving to the database
	return counter++;
}

//TODO: (1) creature load, save

void CreatureManager::Unload(int uid) {
	runHook(lua, unloadRef, &elementMap.find(uid)->second, uid);
	elementMap.erase(uid);
}

void CreatureManager::UnloadAll() {
	for (std::map<int, CreatureData>::iterator it = elementMap.begin(); it != elementMap.end(); it++) {
		runHook(lua, unloadRef, &it->second, it->first);
	}
	elementMap.clear();
}

void CreatureManager::UnloadIf(std::function<bool(std::pair<const int, CreatureData const&>)> fn) {
	std::map<int, CreatureData>::iterator it = elementMap.begin();
	while (it != elementMap.end()) {
		if (fn(*it)) {
			runHook(lua, unloadRef, &it->second, it->first);
			it = elementMap.erase(it);
		}
		else {
			++it;
		}
	}
}

CreatureData* CreatureManager::Find(int uid) {
	std::map<int, CreatureData>::iterator it = elementMap.find(uid);

	if (it == elementMap.end()) {
		return nullptr;
	}

	return &it->second;
}

int CreatureManager::GetLoadedCount() {
	return elementMap.size();
}

std::map<int, CreatureData>* CreatureManager::GetContainer() {
	return &elementMap;
}

lua_State* CreatureManager::SetLuaState(lua_State* L) {
	return lua = L;
}

lua_State* CreatureManager::GetLuaState() {
	return lua;
}

sqlite3* CreatureManager::SetDatabase(sqlite3* db) {
	return database = db;
}

sqlite3* CreatureManager::GetDatabase() {
	return database;
}

int CreatureManager::SetCreateReference(int i) {
	return createRef = i;
}

int CreatureManager::SetUnloadReference(int i) {
	return unloadRef = i;
}

int CreatureManager::GetCreateReference() {
	return createRef;
}

int CreatureManager::GetUnloadReference() {
	return unloadRef;
}

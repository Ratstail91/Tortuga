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
#include "creature_manager_api.hpp"

#include "creature_manager.hpp"

//args: mgr, avatar, script
static int create(lua_State* L) {
	//register the function at the top of the stack
	lua_pushinteger(L, luaL_ref(L, LUA_REGISTRYINDEX));

	//create the actual creature
	CreatureManager* mgr = static_cast<CreatureManager* const>(lua_touserdata(L, 1));
	int index = mgr->Create(lua_tostring(L, 2), lua_tointeger(L, 3)); //3 should be the unique reference within LUA_REGISTRYINDEX
	CreatureData* creature = mgr->Find(index);
	lua_pushlightuserdata(L, static_cast<void*>(creature));
	lua_pushinteger(L, index);
	return 2;
}

//TOOD: overload this to take the userdata as a parameter
static int unload(lua_State* L) {
	CreatureManager* mgr = static_cast<CreatureManager* const>(lua_touserdata(L, 1));
	mgr->Unload(lua_tointeger(L, 2));
	return 0;
}

static int unloadAll(lua_State* L) {
	CreatureManager* mgr = static_cast<CreatureManager* const>(lua_touserdata(L, 1));
	mgr->UnloadAll();
	return 0;
}

static int unloadIf(lua_State* L) {
	CreatureManager* mgr = static_cast<CreatureManager* const>(lua_touserdata(L, 1));

	//list of stuff to unload (don't invalidate iterators)
	std::list<int> unloadList;

	//unloadIf
	for (auto it : *mgr->GetContainer()) {
		//copy the function at the top
		lua_pushvalue(L, -1);

		//index & object as function parameters
		lua_pushinteger(L, it.first);
		lua_pushlightuserdata(L, &it.second);

		//call
		lua_pcall(L, 2, 1, 0);

		//unload-ish
		if (lua_toboolean(L, -1)) {
			unloadList.push_back(it.first);
		}
	}

	//actually unload
	for (auto& it : unloadList) {
		mgr->Unload(it);
	}
	return 0;
}

static int find(lua_State* L) {
	CreatureManager* mgr = static_cast<CreatureManager* const>(lua_touserdata(L, 1));
	CreatureData* creature = mgr->Find(lua_tointeger(L, 2));
	lua_pushlightuserdata(L, static_cast<void*>(creature));
	return 1;
}

static int getLoadedCount(lua_State* L) {
	CreatureManager* mgr = static_cast<CreatureManager* const>(lua_touserdata(L, 1));
	lua_pushinteger(L, mgr->GetLoadedCount());
	return 1;
}

static int setOnCreate(lua_State* L) {
	CreatureManager* mgr = static_cast<CreatureManager*>(lua_touserdata(L, 1));
	mgr->SetCreateReference(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

static int setOnUnload(lua_State* L) {
	CreatureManager* mgr = static_cast<CreatureManager*>(lua_touserdata(L, 1));
	mgr->SetUnloadReference(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

static const luaL_Reg creatureManagerLib[] = {
	{"Create", create},
	{"Unload", unload},
	{"UnloadAll", unloadAll},
	{"UnloadIf", unloadIf},
	{"Find", find},
	{"GetLoadedCount", getLoadedCount},
	{"SetOnCreate", setOnCreate},
	{"SetOnUnload", setOnUnload},
	{nullptr, nullptr}
};

LUAMOD_API int openCreatureManagerAPI(lua_State* L) {
	luaL_newlib(L, creatureManagerLib);
	return 1;
}
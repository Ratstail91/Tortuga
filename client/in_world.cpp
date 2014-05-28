/* Copyright: (c) Kayne Ruse 2013, 2014
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

#include <algorithm>
#include <cmath>
#include <stdexcept>

//-------------------------
//Public access members
//-------------------------

InWorld::InWorld(
	ConfigUtility* const argConfig,
	UDPNetworkUtility* const argNetwork,
	int* const argClientIndex,
	int* const argAccountIndex,
	int* const argCharacterIndex,
	std::map<int, CombatData>* argCombatMap,
	std::map<int, CharacterData>* argCharacterMap
	):
	config(*argConfig),
	network(*argNetwork),
	clientIndex(*argClientIndex),
	accountIndex(*argAccountIndex),
	characterIndex(*argCharacterIndex),
	combatMap(*argCombatMap),
	characterMap(*argCharacterMap)
{
	//setup the utility objects
	buttonImage.LoadSurface(config["dir.interface"] + "button_menu.bmp");
	buttonImage.SetClipH(buttonImage.GetClipH()/3);
	font.LoadSurface(config["dir.fonts"] + "pk_white_8.bmp");

	//pass the utility objects
	disconnectButton.SetImage(&buttonImage);
	disconnectButton.SetFont(&font);
	shutDownButton.SetImage(&buttonImage);
	shutDownButton.SetFont(&font);

	//set the button positions
	disconnectButton.SetX(50);
	disconnectButton.SetY(50 + buttonImage.GetClipH() * 0);
	shutDownButton.SetX(50);
	shutDownButton.SetY(50 + buttonImage.GetClipH() * 1);

	//set the button texts
	disconnectButton.SetText("Disconnect");
	shutDownButton.SetText("Shut Down");

	//load the tilesheet
	//TODO: add the tilesheet to the map system?
	tileSheet.Load(config["dir.tilesets"] + "terrain.bmp", 12, 15);

	//request a sync
	RequestSynchronize();

	//debug
//	RequestRegion(0, 0);
}

InWorld::~InWorld() {
	//
}

//-------------------------
//Frame loop
//-------------------------

void InWorld::FrameStart() {
	//
}

void InWorld::Update(double delta) {
	SerialPacket packet;

	//suck in all waiting packets
	while(network.Receive(&packet)) {
		HandlePacket(packet);
	}

	//update the characters
	for (auto& it : characterMap) {
		it.second.Update(delta);
	}

	//update the camera
	if(localCharacter) {
		camera.x = localCharacter->position.x - camera.marginX;
		camera.y = localCharacter->position.y - camera.marginY;
	}

	//check the map
	UpdateMap();
}

void InWorld::FrameEnd() {
	//
}

void InWorld::RenderFrame() {
//	SDL_FillRect(GetScreen(), 0, 0);
	Render(GetScreen());
	SDL_Flip(GetScreen());
	fps.Calculate();
}

void InWorld::Render(SDL_Surface* const screen) {
	//draw the map
	for (auto it = regionPager.GetContainer()->begin(); it != regionPager.GetContainer()->end(); it++) {
		tileSheet.DrawRegionTo(screen, *it, camera.x, camera.y);
	}

	//draw characters
	for (auto& it : characterMap) {
		//TODO: drawing order according to Y position
		it.second.DrawTo(screen, camera.x, camera.y);
	}

	//draw UI
	disconnectButton.DrawTo(screen);
	shutDownButton.DrawTo(screen);
	font.DrawStringTo(to_string_custom(fps.GetFrameRate()), screen, 0, 0);
}

//-------------------------
//Event handlers
//-------------------------

void InWorld::QuitEvent() {
	//exit the game AND the server
	RequestDisconnect();
	SetNextScene(SceneList::MAINMENU);
}

void InWorld::MouseMotion(SDL_MouseMotionEvent const& motion) {
	disconnectButton.MouseMotion(motion);
	shutDownButton.MouseMotion(motion);
}

void InWorld::MouseButtonDown(SDL_MouseButtonEvent const& button) {
	disconnectButton.MouseButtonDown(button);
	shutDownButton.MouseButtonDown(button);
}

void InWorld::MouseButtonUp(SDL_MouseButtonEvent const& button) {
	if (disconnectButton.MouseButtonUp(button) == Button::State::HOVER) {
		RequestDisconnect();
	}
	if (shutDownButton.MouseButtonUp(button) == Button::State::HOVER) {
		RequestShutDown();
	}
}

void InWorld::KeyDown(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		case SDLK_ESCAPE: {
			QuitEvent();
		}
		break;

		//player movement
		case SDLK_LEFT:
			if (localCharacter) {
				localCharacter->motion.x -= CHARACTER_WALKING_SPEED;
				SendPlayerUpdate();
			}
		break;

		case SDLK_RIGHT:
			if (localCharacter) {
				localCharacter->motion.x += CHARACTER_WALKING_SPEED;
				SendPlayerUpdate();
			}
		break;

		case SDLK_UP:
			if (localCharacter) {
				localCharacter->motion.y -= CHARACTER_WALKING_SPEED;
				SendPlayerUpdate();
			}
		break;

		case SDLK_DOWN:
			if (localCharacter) {
				localCharacter->motion.y += CHARACTER_WALKING_SPEED;
				SendPlayerUpdate();
			}
		break;
	}
}

void InWorld::KeyUp(SDL_KeyboardEvent const& key) {
	switch(key.keysym.sym) {
		//player movement
		case SDLK_LEFT:
			if (localCharacter) {
				localCharacter->motion.x += CHARACTER_WALKING_SPEED;
				SendPlayerUpdate();
			}
		break;

		case SDLK_RIGHT:
			if (localCharacter) {
				localCharacter->motion.x -= CHARACTER_WALKING_SPEED;
				SendPlayerUpdate();
			}
		break;

		case SDLK_UP:
			if (localCharacter) {
				localCharacter->motion.y += CHARACTER_WALKING_SPEED;
				SendPlayerUpdate();
			}
		break;

		case SDLK_DOWN:
			if (localCharacter) {
				localCharacter->motion.y -= CHARACTER_WALKING_SPEED;
				SendPlayerUpdate();
			}
		break;
	}
}

//-------------------------
//Network handlers
//-------------------------

void InWorld::HandlePacket(SerialPacket packet) {
	switch(packet.meta.type) {
		case SerialPacket::Type::DISCONNECT:
			HandleDisconnect(packet);
		break;
		case SerialPacket::Type::REGION_CONTENT:
			HandleRegionContent(packet);
		break;
		case SerialPacket::Type::CHARACTER_UPDATE:
			HandleCharacterUpdate(packet);
		break;
		case SerialPacket::Type::CHARACTER_NEW:
			HandleCharacterNew(packet);
		break;
		case SerialPacket::Type::CHARACTER_DELETE:
			HandleCharacterDelete(packet);
		break;
		//handle errors
		default:
			throw(std::runtime_error(std::string() + "Unknown SerialPacket::Type encountered in InWorld: " + to_string_custom(int(packet.meta.type))));
		break;
	}
}

void InWorld::HandleDisconnect(SerialPacket packet) {
	network.Unbind(Channels::SERVER);
	clientIndex = -1;
	accountIndex = -1;
	characterIndex = -1;
	SetNextScene(SceneList::MAINMENU);
}

void InWorld::HandleRegionContent(SerialPacket packet) {
	//replace existing regions
	regionPager.UnloadRegion(packet.regionInfo.x, packet.regionInfo.y);
	regionPager.PushRegion(packet.regionInfo.region);
	packet.regionInfo.region = nullptr;
}

void InWorld::HandleCharacterUpdate(SerialPacket packet) {
	if (characterMap.find(packet.characterInfo.characterIndex) == characterMap.end()) {
		HandleCharacterNew(packet);
		return;
	}

	//update only if the message didn't originate from here
	if (packet.characterInfo.clientIndex != clientIndex) {
		characterMap[packet.characterInfo.characterIndex].position = packet.characterInfo.position;
		characterMap[packet.characterInfo.characterIndex].motion = packet.characterInfo.motion;
	}
	characterMap[packet.characterInfo.characterIndex].CorrectSprite();
}

void InWorld::HandleCharacterNew(SerialPacket packet) {
	if (characterMap.find(packet.characterInfo.characterIndex) != characterMap.end()) {
		throw(std::runtime_error("Cannot create duplicate characters"));
	}

	//create the character object
	CharacterData& character = characterMap[packet.characterInfo.characterIndex];

	//set the members
	character.handle = packet.characterInfo.handle;
	character.avatar = packet.characterInfo.avatar;
	character.sprite.LoadSurface(config["dir.sprites"] + character.avatar, 4, 4);
	character.mapIndex = packet.characterInfo.mapIndex;
	character.position = packet.characterInfo.position;
	character.motion = packet.characterInfo.motion;
	character.stats = packet.characterInfo.stats;

	character.CorrectSprite();

	//catch this client's player object
	if (packet.characterInfo.characterIndex == characterIndex && !localCharacter) {
		localCharacter = &character;

		//setup the camera
		//TODO: can't change the screen size?
		camera.width = GetScreen()->w;
		camera.height = GetScreen()->h;

		//center on the player's character
		camera.marginX = (GetScreen()->w / 2 - localCharacter->sprite.GetImage()->GetClipW() / 2);
		camera.marginY = (GetScreen()->h / 2 - localCharacter->sprite.GetImage()->GetClipH() / 2);
	}
}

void InWorld::HandleCharacterDelete(SerialPacket packet) {
	//TODO: authenticate when own character is being deleted (linked to a TODO in the server)
	//catch this client's player object
	if (packet.characterInfo.characterIndex == characterIndex) {
		characterIndex = -1;
		localCharacter = nullptr;
	}

	characterMap.erase(packet.characterInfo.characterIndex);
}

//-------------------------
//Server control
//-------------------------

void InWorld::RequestSynchronize() {
	SerialPacket packet;

	//request a sync
	packet.meta.type = SerialPacket::Type::SYNCHRONIZE;
	packet.clientInfo.clientIndex = clientIndex;
	packet.clientInfo.accountIndex = accountIndex;
	packet.clientInfo.characterIndex = characterIndex;

	network.SendTo(Channels::SERVER, &packet);
}

void InWorld::SendPlayerUpdate() {
	SerialPacket packet;

	//pack the packet
	packet.meta.type = SerialPacket::Type::CHARACTER_UPDATE;
	packet.characterInfo.clientIndex = clientIndex;
	packet.characterInfo.accountIndex = accountIndex;
	packet.characterInfo.characterIndex = characterIndex;
	packet.characterInfo.position = localCharacter->position;
	packet.characterInfo.motion = localCharacter->motion;

	network.SendTo(Channels::SERVER, &packet);
}

void InWorld::RequestDisconnect() {
	SerialPacket packet;

	//send a disconnect request
	packet.meta.type = SerialPacket::Type::DISCONNECT;
	packet.clientInfo.clientIndex = clientIndex;
	packet.clientInfo.accountIndex = accountIndex;
	packet.clientInfo.characterIndex = characterIndex;

	network.SendTo(Channels::SERVER, &packet);
}

void InWorld::RequestShutDown() {
	SerialPacket packet;

	//send a shutdown request
	packet.meta.type = SerialPacket::Type::SHUTDOWN;
	packet.clientInfo.clientIndex = clientIndex;
	packet.clientInfo.accountIndex = accountIndex;
	packet.clientInfo.characterIndex = characterIndex;

	network.SendTo(Channels::SERVER, &packet);
}

void InWorld::RequestRegion(int mapIndex, int x, int y) {
	SerialPacket packet;

	//pack the region's data
	packet.meta.type = SerialPacket::Type::REGION_REQUEST;
	packet.regionInfo.mapIndex = mapIndex;
	packet.regionInfo.x = x;
	packet.regionInfo.y = y;

	network.SendTo(Channels::SERVER, &packet);
}

//-------------------------
//Utilities
//-------------------------

//TODO: convert this into a more generic function?; using parameters for the bounds
void InWorld::UpdateMap() {
	//these represent the zone of regions that the client needs loaded, including the mandatory buffers (+1/-1)
	int xStart = snapToBase(REGION_WIDTH, camera.x/tileSheet.GetTileW()) - REGION_WIDTH;
	int xEnd = snapToBase(REGION_WIDTH, (camera.x+camera.width)/tileSheet.GetTileW()) + REGION_WIDTH;

	int yStart = snapToBase(REGION_HEIGHT, camera.y/tileSheet.GetTileH()) - REGION_HEIGHT;
	int yEnd = snapToBase(REGION_HEIGHT, (camera.y+camera.height)/tileSheet.GetTileH()) + REGION_HEIGHT;

	//prune distant regions
	for (auto it = regionPager.GetContainer()->begin(); it != regionPager.GetContainer()->end(); /* EMPTY */) {
		//check if the region is outside off this area
		if ((*it)->GetX() < xStart || (*it)->GetX() > xEnd || (*it)->GetY() < yStart || (*it)->GetY() > yEnd) {

			//clunky, but the alternative was time consuming
			int tmpX = (*it)->GetX();
			int tmpY = (*it)->GetY();
			++it;

			regionPager.UnloadRegion(tmpX, tmpY);
			continue;
		}
		++it;
	}

	//request empty regions within this zone
	for (int i = xStart; i <= xEnd; i += REGION_WIDTH) {
		for (int j = yStart; j <= yEnd; j += REGION_HEIGHT) {
			if (!regionPager.FindRegion(i, j)) {
				RequestRegion(0, i, j);
			}
		}
	}
}

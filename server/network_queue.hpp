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
#ifndef NETWORKQUEUE_HPP_
#define NETWORKQUEUE_HPP_

#include "network_packet.hpp"

#include "SDL/SDL_thread.h"

#include <deque>

class NetworkQueue {
public:
	NetworkQueue();
	~NetworkQueue();

	NetworkPacket Push(NetworkPacket);
	NetworkPacket Peek();
	NetworkPacket Pop();
	void Flush();

	int Size();

	SDL_sem* GetLock() const { return lock; }
private:
	std::deque<NetworkPacket> queue;
	SDL_sem* lock;
};

#endif

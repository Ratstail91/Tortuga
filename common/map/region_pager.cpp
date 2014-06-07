/* Copyright: (c) Kayne Ruse 2014
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
#include "region_pager.hpp"

#include "utility.hpp"

Region::type_t RegionPager::SetTile(int x, int y, int z, Region::type_t v) {
	Region* ptr = GetRegion(x, y);
	return ptr->SetTile(x - ptr->GetX(), y - ptr->GetY(), z, v);
}

Region::type_t RegionPager::GetTile(int x, int y, int z) {
	Region* ptr = GetRegion(x, y);
	return ptr->GetTile(x - ptr->GetX(), y - ptr->GetY(), z);
}

Region* RegionPager::GetRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//get the region by various means
	Region* ptr = nullptr;
	ptr = FindRegion(x, y);
	if (ptr) return ptr;
	ptr = LoadRegion(x, y);
	if (ptr) return ptr;
	return CreateRegion(x, y);
}

Region* RegionPager::FindRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//find the region
	for (std::list<Region*>::iterator it = regionList.begin(); it != regionList.end(); it++) {
		if ((*it)->GetX() == x && (*it)->GetY() == y) {
			return *it;
		}
	}
	return nullptr;
}

Region* RegionPager::LoadRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//load the region if possible
	//TODO: Load the region (lua)
	return nullptr;
}

Region* RegionPager::SaveRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//find & save the region
	Region* ptr = FindRegion(x, y);
	if (ptr) {
		//TODO: save the region (lua)
	}
	return ptr;
}

Region* RegionPager::CreateRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//create and push the object
	Region* ptr = new Region(x, y);
	//TODO: create the region (lua)
	regionList.push_front(ptr);
	return ptr;
}

void RegionPager::UnloadRegion(int x, int y) {
	//snap the coords
	x = snapToBase(REGION_WIDTH, x);
	y = snapToBase(REGION_HEIGHT, y);

	//custom loop, not FindRegion()
	for (std::list<Region*>::iterator it = regionList.begin(); it != regionList.end(); /* EMPTY */) {
		if ((*it)->GetX() == x && (*it)->GetY() == y) {
			//TODO: unload the region (lua)
			delete (*it);
			it = regionList.erase(it);
			continue;
		}
		++it;
	}
}

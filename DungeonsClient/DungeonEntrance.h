#pragma once
#include "Util.h"
#include "Biome.h"

class DungeonEntrance
{
public:
	DungeonEntrance();
	DungeonEntrance(DungeonType dungeonType_, pi pos_);

	ID_t gameStateID;
	DungeonType dungeonType;
	pi pos;
private:
	static ID_t idd;
};


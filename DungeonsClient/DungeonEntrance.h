#pragma once
#include "Util.h"
#include "Biome.h"

class DungeonEntrance
{
public:
	DungeonEntrance();
	DungeonEntrance(BiomeType biomeType_, pi pos_);

	ID_t gameStateID;
	BiomeType biomeType;
	pi pos;
private:
	static ID_t idd;
};


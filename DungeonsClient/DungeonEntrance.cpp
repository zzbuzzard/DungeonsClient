#include "pch.h"
#include "DungeonEntrance.h"

DungeonEntrance::DungeonEntrance() {}

DungeonEntrance::DungeonEntrance(BiomeType biomeType_, pi pos_) 
	: gameStateID(-1), biomeType(biomeType_), pos(pos_)
{
}

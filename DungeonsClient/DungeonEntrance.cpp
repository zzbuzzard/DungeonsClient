#include "pch.h"
#include "DungeonEntrance.h"

DungeonEntrance::DungeonEntrance() {}

DungeonEntrance::DungeonEntrance(DungeonType dungeonType_, pi pos_) 
	: gameStateID(-1), dungeonType(dungeonType_), pos(pos_)
{
}

#pragma once
#include "DungeonGenerator.h"
class GrasslandsDungeonGenerator : public DungeonGenerator
{
public:
	GrasslandsDungeonGenerator(World *world);
protected:
	void drawRoom(DNode* node);

private:
	void drawBoss(DNode *node);
	void drawSpawn(DNode *node);
	void drawPlantRoom(DNode *node);
	void drawEntRoom(DNode *node);
	void drawForestRoom(DNode *node);
	void drawClearing(DNode *node);
	void drawMudRoom(DNode *node);
};

// Grass enemies
//  Muncher
//  Dark muncher
//  Eye spore
//  Elf archer, knight, cursed knight
//  Leafling
//  Ent
//  Spider

// Room types
//  Plant room: eye spores, munchers, leaflings
//  Ent room: One ent per room, dramatically in the center
//  Forest room: TREE OBSTACLES, elves, spiders
//  Clearing: empty
//  Mud room: Slimes, spiders
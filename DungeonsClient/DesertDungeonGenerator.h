#pragma once
#include "DungeonGenerator.h"
class DesertDungeonGenerator : public DungeonGenerator
{
public:
	DesertDungeonGenerator(World *world);
protected:
	void drawRoom(DNode* node);

	//void drawSpawn(Node *node);
	void drawBoss(DNode *node);
	void drawBlocky(DNode *node);
	void drawBarren(DNode *node);
	void drawTankRoom(DNode *node);
	void drawNormal(DNode *node);
};


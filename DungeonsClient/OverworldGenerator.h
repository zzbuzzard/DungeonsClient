#pragma once
#include "MapGenerator.h"
#include "Util.h"
#include "Biome.h"
#include <random>
#include <vector>
#include <map>

struct OG_Node;

class OverworldGenerator : public MapGenerator
{
public:
	OverworldGenerator(World *world, WorldType worldType_);
	bool generate();
private:
	WorldType worldType;

	void generateFrom(OG_Node *node, float left, float right);
	pi drawFrom(OG_Node *node, pi start);
	void drawOutline();
	void deallocateFrom(OG_Node *node);
};


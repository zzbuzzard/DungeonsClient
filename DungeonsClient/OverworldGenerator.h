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
	OverworldGenerator(World *world);
	bool generate();
private:
	void generateFrom(OG_Node *node, float left, float right, int *dungeons, bool last);
	void drawFrom(OG_Node *node);
	BiomeType getBaseBiome();
	void drawOutline();
	void deallocateFrom(OG_Node *node);
};


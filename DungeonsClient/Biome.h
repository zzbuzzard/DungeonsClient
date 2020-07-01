#pragma once
#include <vector>
#include "Tile.h"
#include "LivingEntity.h"

enum class BiomeType {
	DESERT,
	SNOW,
	GRASS,
	VOLCANO,

	NONE = -1
};

class Biome {
public:
	Biome(Tile biomeTile_, const spawnVector &spawns_, TEXTURE doorTexture_);

	Tile biomeTile;
	spawnVector spawns;
	TEXTURE doorTexture;
};

#define NUM_BIOME 4
extern Biome biomes[NUM_BIOME];

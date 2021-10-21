#pragma once
#include "Media.h"

#define NUM_TILES 14
enum class Tile {
	NONE,
	WALL,
	WALL_WOOD,
	SAND,

	GRASS,
	SNOW,
	VOLCANO,
	DARK_SAND,

	GRASS2,
	GRASS_DARK,
	GRASS_DARK_BLOOD,
	GRASS_BLOOD,

	MUD,
	DIRT,
};

extern TEXTURE tileTextures[NUM_TILES];
extern bool tileWalkable[NUM_TILES];
extern sf::Color tileMinimapColour[NUM_TILES];


typedef std::pair<Tile, float> TileSpawnPair;
typedef std::vector<TileSpawnPair> TileSpawnVector;

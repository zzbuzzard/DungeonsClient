#pragma once
#include "Media.h"

#define NUM_TILES 12
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
	GRASS_BLOOD,
	MUD,
};

extern TEXTURE tileTextures[NUM_TILES];
extern bool tileWalkable[NUM_TILES];
extern sf::Color tileMinimapColour[NUM_TILES];
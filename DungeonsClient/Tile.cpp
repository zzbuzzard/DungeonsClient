#include "pch.h"
#include "Tile.h"

//NONE,
//SAND,
//WALL,
//GRASS,
//SNOW,
//VOLCANO,
//DARK_SAND

TEXTURE tileTextures[NUM_TILES] = {
	T_BLACK,
	T_TILE_WALL,
	T_TILE_WALL_WOOD,
	T_TILE_SAND,

	T_TILE_GRASS,
	T_TILE_SNOW,
	T_TILE_VOLCANO,
	T_TILE_DARK_SAND,

	T_TILE_GRASS2,
	T_TILE_GRASS_DARK,
	T_TILE_GRASS_DARK_BLOOD,
	T_TILE_GRASS_BLOOD,

	T_TILE_MUD,
	T_TILE_DIRT,
};


bool tileWalkable[NUM_TILES] = {
	false, //Tile::NONE
	false, //Tile::WALL
	false,
	true,

	true,
	true,
	true,
	true,

	true,
	true,
	true,
	true,

	true,
	true,
};

sf::Color tileMinimapColour[NUM_TILES] = {
	sf::Color::Black,
	sf::Color(50, 50, 50),
	sf::Color(40, 20, 0),
	sf::Color(240, 240, 0),

	sf::Color(0, 200, 0),
	sf::Color(225, 225, 225),
	sf::Color(150, 0, 0),
	sf::Color(170, 170, 0),

	sf::Color(100, 200, 0),
	sf::Color(20, 150, 20),
	sf::Color(50, 100, 20),
	sf::Color(80, 5, 5),

	sf::Color(100, 50, 0),
	sf::Color(170, 120, 20),
};
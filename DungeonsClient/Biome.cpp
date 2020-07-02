#include "pch.h"
#include "Biome.h"
#include "Media.h"

Biome biomes[NUM_BIOME] = {
	Biome(Tile::SAND, {
	spawnPair(E_ENEMY1, 1.0f), spawnPair(E_ENEMY2, 0.8f), spawnPair(E_SCORPION, 0.2f), spawnPair(E_SAND_GOLEM, 0.01f), spawnPair(E_SCORPION, 0.1f)}, T_DUNGEON_ENTRANCE_DESERT),
	
	Biome(Tile::SNOW, {}, T_DUNGEON_ENTRANCE_DESERT),
	
	Biome(Tile::GRASS, {
	spawnPair(E_MUNCHER, 1.0f), spawnPair(E_EYE_SPORE, 0.8f), spawnPair(E_DARK_MUNCHER, 0.01f), spawnPair(E_SPIDER, 0.8f), spawnPair(E_EYE_SPORE, 0.5f) }, T_DUNGEON_ENTRANCE_GRASS),
	
	Biome(Tile::VOLCANO, {}, T_DUNGEON_ENTRANCE_DESERT),
};


Biome::Biome(Tile biomeTile_, const spawnVector &spawns_, TEXTURE doorTexture_)
	: biomeTile(biomeTile_), spawns(spawns_), doorTexture(doorTexture_) {
}

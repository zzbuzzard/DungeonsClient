#include "pch.h"
#include "Biome.h"
#include "Media.h"
#include "World.h"

DungeonTypeData::DungeonTypeData(std::string dungeonName_, const spawnVector &overworldSpawns_, TEXTURE doorTexture_,
	const tlTileSpawnVector &overworldTlSpawns_, const TileSpawnVector &defaultTiles_)

	: dungeonName(dungeonName_), overworldSpawns(overworldSpawns_), doorTexture(doorTexture_),
		overworldTlSpawns(overworldTlSpawns_), defaultTiles(defaultTiles_)
{

}

DungeonTypeData dungeonTypeData[DUNGEON_TYPE_NUM] = {
	DungeonTypeData("Sand dungeon",
		{spawnPair(E_ENEMY1, 1.0f), spawnPair(E_ENEMY2, 0.8f), spawnPair(E_SCORPION, 0.2f),
		spawnPair(E_SAND_GOLEM, 0.01f), spawnPair(E_SCORPION, 0.1f), },
		T_DUNGEON_ENTRANCE_DESERT,
		{ tlTileSpawnPair(TLTileType::CACTUS, 0.02f), tlTileSpawnPair(TLTileType::NONE, 0.98f) },
		{ TileSpawnPair(Tile::SAND, 1.0f) }), // TileSpawnPair(Tile::DARK_SAND, 0.1f)

	DungeonTypeData("Forest labyrinth",
		{spawnPair(E_MUNCHER, 1.0f), spawnPair(E_EYE_SPORE, 0.8f), spawnPair(E_DARK_MUNCHER, 0.01f),
		spawnPair(E_SPIDER, 0.8f) },
		T_DUNGEON_ENTRANCE_GRASS,
		{ tlTileSpawnPair(TLTileType::BUSH, 0.04f), tlTileSpawnPair(TLTileType::NONE, 0.96f) }, 
		{ TileSpawnPair(Tile::GRASS_DARK_BLOOD, 0.05f), TileSpawnPair(Tile::GRASS_DARK, 0.95f) }),
};

WorldTypeData::WorldTypeData(std::string worldName_, const TileSpawnVector &tileSpawns_, const dungeonVector &dungeonSpawns_)
	: worldName(worldName_), tileSpawns(tileSpawns_), dungeonSpawns(dungeonSpawns_)
{
}


DungeonType WorldTypeData::getRandomDungeon(World *world) const {
	return world->randomFromWeightedVector(dungeonSpawns);
}

WorldTypeData worldTypeData[WORLD_TYPE_NUM] = {
	WorldTypeData("Desert",
		{ TileSpawnPair(Tile::SAND, 0.95f), TileSpawnPair(Tile::DARK_SAND, 0.05f) },
		{ dungeonPair(DungeonType::DESERT, 1.0f) }),
	
	WorldTypeData("Snow",
		{ TileSpawnPair(Tile::SNOW, 1.0f) },
		{ dungeonPair(DungeonType::GRASS, 0.5f), dungeonPair(DungeonType::DESERT, 0.5f) }),
	
	WorldTypeData("Grass",
		{ TileSpawnPair(Tile::GRASS, 0.75f), TileSpawnPair(Tile::GRASS2, 0.15f), TileSpawnPair(Tile::GRASS_DARK, 0.1f) },
		{ dungeonPair(DungeonType::GRASS, 1.0f) }),
};




//Biome biomes[NUM_BIOME] = {
//	Biome(Tile::SAND,
//		{spawnPair(E_ENEMY1, 1.0f), spawnPair(E_ENEMY2, 0.8f), spawnPair(E_SCORPION, 0.2f), spawnPair(E_SAND_GOLEM, 0.01f), spawnPair(E_SCORPION, 0.1f)},
//		{tileSpawnPair(TLTileType::CACTUS, 0.02f), tileSpawnPair(TLTileType::NONE, 0.98f)},
//		T_DUNGEON_ENTRANCE_DESERT),
//	
//	Biome(Tile::SNOW,
//		{},
//		{},
//		T_DUNGEON_ENTRANCE_DESERT),
//	
//	Biome(Tile::GRASS,
//		{spawnPair(E_MUNCHER, 1.0f), spawnPair(E_EYE_SPORE, 0.8f), spawnPair(E_DARK_MUNCHER, 0.01f), spawnPair(E_SPIDER, 0.8f) },
//		{tileSpawnPair(TLTileType::BUSH, 0.03f), tileSpawnPair(TLTileType::NONE, 0.97f)},
//		T_DUNGEON_ENTRANCE_GRASS),
//	
//	Biome(Tile::VOLCANO,
//		{},
//		{},
//		T_DUNGEON_ENTRANCE_DESERT),
//};
//
//
//Biome::Biome(Tile biomeTile_, const spawnVector &spawns_, const tileSpawnVector &tileSpawns_, TEXTURE doorTexture_)
//	: biomeTile(biomeTile_), spawns(spawns_), doorTexture(doorTexture_), tileSpawns(tileSpawns_) {
//}

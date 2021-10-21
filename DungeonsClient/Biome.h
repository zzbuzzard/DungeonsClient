#pragma once
#include <vector>
#include "Tile.h"
#include "LivingEntity.h"
#include "TopLayerTile.h"
#include <string>

class World;


////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            Dungeon                                                 //
////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class DungeonType {
	DESERT,
	GRASS,
};

class DungeonTypeData {
public:
	DungeonTypeData(std::string dungeonName_, const spawnVector &overworldSpawns_, TEXTURE doorTexture_,
					const tlTileSpawnVector &overworldTlSpawns_, const TileSpawnVector &defaultTiles);

	const std::string dungeonName;
	const spawnVector overworldSpawns;
	const tlTileSpawnVector overworldTlSpawns;
	const TileSpawnVector defaultTiles;

	const TEXTURE doorTexture;
};
#define DUNGEON_TYPE_NUM 2
extern DungeonTypeData dungeonTypeData[DUNGEON_TYPE_NUM];


typedef std::pair<DungeonType, float> dungeonPair;
typedef std::vector<dungeonPair> dungeonVector;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                             World                                                  //
////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class WorldType {
	DESERT,
	SNOW,
	GRASS,
};

class WorldTypeData {
public:
	WorldTypeData(std::string worldName_, const TileSpawnVector &tileSpawns_, const dungeonVector &dungeonSpawns_);

	const std::string worldName;
	const TileSpawnVector tileSpawns;
	const dungeonVector dungeonSpawns;

	DungeonType getRandomDungeon(World *world) const;
};
#define WORLD_TYPE_NUM 3
extern WorldTypeData worldTypeData[WORLD_TYPE_NUM];

////////////////////////////////////////////////////////////////////////////////////////////////////////


//enum class BiomeType {
//	DESERT,
//	SNOW,
//	GRASS,
//	VOLCANO,
//
//	NONE = -1
//};
//
//
//class Biome {
//public:
//	Biome(Tile biomeTile_, const spawnVector &spawns_, const tileSpawnVector &tileSpawns_, TEXTURE doorTexture_);
//
//	Tile biomeTile;
//	spawnVector spawns;
//	tileSpawnVector tileSpawns;
//	TEXTURE doorTexture;
//};
//
//#define NUM_BIOME 4
//extern Biome biomes[NUM_BIOME];

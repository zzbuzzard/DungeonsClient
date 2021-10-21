#pragma once
#include <random>
#include <vector>
#include <set>
#include <map>
#include "CSMode.h"
#include "Util.h"
#include "Media.h"
#include "LivingEntity.h"
#include "Tile.h"
#include "Biome.h"
#include "DungeonEntrance.h"

#ifndef CLIENT
class Spawner;
#endif

class Entity;

class World
{
public:
	ID_t worldID;
	std::vector<std::vector<Tile> > tiles;
	std::map<pi, DungeonEntrance, piComp> dungeonEntrances;

	//void generateTown(seed_t seed_); TODO
	void generateOverworld(seed_t seed_, WorldType worldType);
	void generateDungeon(seed_t seed_, DungeonType biome);

	Tile getTile(pi pos) const;
	bool canWalk(pi pos) const;

	bool isBossRoom(pi pos) const;

	bool initialised = false;
	bool isOverworld = false;

	seed_t seed;
	// The index in the map that represents (0, 0)
	pi origin;

	pi bossRoomBL, bossRoomTR;

	void fillWith(pi pos, Tile t, const std::set<pi, piComp> &border);

	void makeDungeon(pi pos, DungeonType dungeonType);

	void seedRandom();
	std::mt19937 gen;
	std::uniform_real_distribution<float> dist;

	float rand(float a, float b);
	int randint(int a, int b);

	template <typename T> void shuffle(std::vector<T> &vec);
	template <typename T> T randomFromWeightedVector(const std::vector<std::pair<T, float> > &vec);

	bool addPermTopLayerTile(Entity* entity);
	bool addTempTopLayerTile(Entity* entity);
	std::vector<std::vector<Entity*> > topLayerTileArray;
	std::set<pi, piComp> TLset1, TLset2; // set 1 is permanents, set 2 is temporaries

#ifndef CLIENT
	//pi getSpawnPosition() const;

	std::vector<Spawner*> spawners;
	bool bossDefeated = false;

	// For spawns
	void update(GameState *state);
	~World(); //deallocate spawners
	World();
#endif
};
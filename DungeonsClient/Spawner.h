#pragma once

#include "CSMode.h"
#ifndef CLIENT

#include "Util.h"
#include "State.h"
#include "GameState.h"
#include "Enemy.h"
#include "Biome.h"

#include <vector>
#include <memory>

// Each biome has an ordered list of EnemySpecies
// 
// Each edge in the graph has a max number of enemies.
// These are selected in a random range depending on the depth
// 
// We create a spawner for each edge.
// The spawned enemy has a value set which it decrements upon dying.

// maxEnemies is float constant * number of spawn points, the constant being max enemy density

// When spawnCountdown goes off, if enemyCount < maxEnemies, a new enemy type is selected then randomly spawned in spawnPoints

class Spawner {
public:
	Spawner(int maxEnemies_, float spawnCooldown_);
	virtual void update(GameState *state);

	std::shared_ptr<int> enemyCounter;
	//int enemyCount;
protected:
	virtual void spawnEnemy(GameState *state) = 0;

	int maxEnemies;
	float spawnT;
	float spawnCool;

private:
	bool firstUpdate;
};


class OverworldSpawner : public Spawner {
public:
	OverworldSpawner(BiomeType biome_, const std::vector<pi> &spawnPoints_, pi end_, float maxDist_);

	BiomeType biome;
	std::vector<pi> spawnPoints;
	pi end;
	float maxDist;
protected:
	void spawnEnemy(GameState *state);
private:
	float total;
};


class DungeonSpawner : public Spawner {
public:
	DungeonSpawner(const spawnVector *spawns_, pi BL_, pi TR_, int maxEnemies_, float respawnRate = 30.0f);
	pi BL, TR;
	const spawnVector* const spawns;

protected:
	void spawnEnemy(GameState *state);
};

class BossSpawner : public Spawner {
public:
	BossSpawner(EntitySpecies spawn, pi pos);
	void update(GameState *state);

	bool spawned = false;

	std::vector<pi> wallPos;
	Dir wallMoveDir;
	Tile wallTile, normalTile;
protected:
	void spawnEnemy(GameState *state);
private:
	void beginTimer(GameState *state);
	void setWall(GameState *state, bool t);
	bool playerInBossRoom(GameState *state);
	void reset(GameState *state);

	bool checkingForPlayers = true;
	bool bossSpawnTimerActive = false;
	bool bossIsAlive = false;
	bool wallSpawned = false;
	float bossSpawnTimer = 0;

	float checkTimer = 0;

	ID_t bossID;

	pi spawnPos;
	EntitySpecies spec;
};


#endif
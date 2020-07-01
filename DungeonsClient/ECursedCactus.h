#pragma once
#include "Enemy.h"
#ifdef CLIENT
class ECursedCactus : public Enemy
{
public:
	ECursedCactus(pi pos_, ID_t id);
};
#else
class ECursedCactus : public Enemy
{
public:
	ECursedCactus(pi pos_);
	void update(GameState *state);

private:
	int PHASE = 0;
	float p1_spawnCool = 0;
	bool isSpawning = true;
	float p1_isSpawningCool;

	bool p2_isMoving;
	float p2_moveTime;

	pi xRange, yRange; // spawns
};
#endif


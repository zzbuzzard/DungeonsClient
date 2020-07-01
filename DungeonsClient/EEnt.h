#pragma once
#include "Enemy.h"
#ifdef CLIENT
class EEnt : public Enemy
{
public:
	EEnt(pi pos_, ID_t id);
};
#else
#include <memory>
#include <set>

// Used to have 2 phases
// The first involved spawning leaflings
//  (but phase 2 was so SICK I removed phase 1.)
class EEnt : public Enemy
{
public:
	EEnt(pi pos_);
	void update(GameState *state);

private:
	//void spawnLeafling(GameState *state);

	std::set<pi, piComp> placedPis;
	std::vector<pi> lastPlaced;
	float timeTilUpdate = 0;
	float restartTime = 0;
	bool making = false;

	//bool phase2;
	//std::shared_ptr<int> leaflingCount;
	//float respawnTime;
};
#endif



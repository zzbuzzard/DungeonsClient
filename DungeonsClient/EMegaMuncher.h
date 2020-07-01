#pragma once
#include "Enemy.h"

#ifdef CLIENT
class EMegaMuncher : public Enemy
{
public:
	EMegaMuncher(pi pos_, ID_t id);
};
#else
class EMegaMuncher : public Enemy
{
public:
	EMegaMuncher(pi pos_);
	void update(GameState *state);

private:
	int PHASE = 0;

	float p1_changeTime;
	bool p1_isChasing;
	float p1_moveFrontTime;
	std::vector<pi> p1_front;

	std::shared_ptr<int> p2_muncherCount;
	float p2_changeTime;
	bool p2_isChasing;
	float p2_spawnCool;
};
#endif


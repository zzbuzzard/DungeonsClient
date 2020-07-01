#pragma once
#include "Enemy.h"
#ifdef CLIENT
class ELeafling : public Enemy
{
public:
	ELeafling(pi pos_, ID_t id);
};
#else
class ELeafling : public Enemy
{
public:
	ELeafling(pi pos_);
	void update(GameState *state);

	bool tethered = false;
	pi tether;
private:
	pi target = pi(0, 0);
};
#endif


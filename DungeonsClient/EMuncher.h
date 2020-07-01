#pragma once
#include "Enemy.h"

#ifdef CLIENT
class EMuncher : public Enemy
{
public:
	EMuncher(pi pos_, ID_t id);
};
#else
class EMuncher : public Enemy
{
public:
	EMuncher(pi pos_);
	void update(GameState *state);
};
#endif


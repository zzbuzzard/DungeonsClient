#pragma once
#include "Enemy.h"

#ifdef CLIENT
class EMummyBoss : public Enemy
{
public:
	EMummyBoss(pi pos_, ID_t id);
};
#else
class EMummyBoss : public Enemy
{
public:
	EMummyBoss(pi pos_);
	void update(GameState *state);
};
#endif



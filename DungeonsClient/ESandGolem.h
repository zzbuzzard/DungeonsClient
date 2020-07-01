#pragma once
#include "Enemy.h"

#ifdef CLIENT
class ESandGolem : public Enemy
{
public:
	ESandGolem(pi pos_, ID_t id);
};
#else
class ESandGolem : public Enemy
{
public:
	ESandGolem(pi pos_);
	void update(GameState *state);
};
#endif


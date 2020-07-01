#pragma once
#include "Enemy.h"
#ifdef CLIENT
class ECursedElfKnight : public Enemy
{
public:
	ECursedElfKnight(pi pos_, ID_t id);
};
#else
class ECursedElfKnight : public Enemy
{
public:
	ECursedElfKnight(pi pos_);
	void update(GameState *state);
};
#endif


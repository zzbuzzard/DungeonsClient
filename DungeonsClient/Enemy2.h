#pragma once
#include "Enemy.h"

#ifdef CLIENT
class Enemy2 : public Enemy
{
public:
	Enemy2(pi pos_, ID_t id);
};
#else
class Enemy2 : public Enemy
{
public:
	Enemy2(pi pos_);
	void update(GameState *state);
};
#endif


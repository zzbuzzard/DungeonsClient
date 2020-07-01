#pragma once
#include "Enemy.h"

#ifdef CLIENT
class Enemy1 : public Enemy
{
public:
	Enemy1(pi pos_, ID_t id);
};
#else
class Enemy1 : public Enemy
{
public:
	Enemy1(pi pos_);
	void update(GameState *state);
};
#endif


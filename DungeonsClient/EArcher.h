#pragma once
#include "Enemy.h"
#ifdef CLIENT
class EArcher : public Enemy
{
public:
	EArcher(pi pos_, ID_t id);
};
#else
class EArcher : public Enemy
{
public:
	EArcher(pi pos_);
	void update(GameState *state);
};
#endif


#pragma once
#include "Enemy.h"

#ifdef CLIENT
class EDarkMuncher : public Enemy
{
public:
	EDarkMuncher(pi pos_, ID_t id);
};
#else
class EDarkMuncher : public Enemy
{
public:
	EDarkMuncher(pi pos_);
	void update(GameState *state);
};
#endif


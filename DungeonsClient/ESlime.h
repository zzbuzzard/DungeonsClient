#pragma once
#include "Enemy.h"
#ifdef CLIENT
class ESlime : public Enemy
{
public:
	ESlime(pi pos_, ID_t id);
};
#else
class ESlime : public Enemy
{
public:
	ESlime(pi pos_);
	void update(GameState *state);
};
#endif


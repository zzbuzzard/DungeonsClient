#pragma once
#include "Enemy.h"

#ifdef CLIENT
class EIceEnemy : public Enemy
{
public:
	EIceEnemy(pi pos_, ID_t id);
};
#else
class EIceEnemy : public Enemy
{
public:
	EIceEnemy(pi pos_);
	void update(GameState *state);
};
#endif




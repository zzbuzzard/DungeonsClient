#pragma once
#include "Enemy.h"
#ifdef CLIENT
class EElfKnight : public Enemy
{
public:
	EElfKnight(pi pos_, ID_t id);
};
#else
class EElfKnight : public Enemy
{
public:
	EElfKnight(pi pos_);
	void update(GameState *state);
};
#endif


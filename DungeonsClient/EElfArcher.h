#pragma once
#include "Enemy.h"
#ifdef CLIENT
class EElfArcher : public Enemy
{
public:
	EElfArcher(pi pos_, ID_t id);
};
#else
class EElfArcher : public Enemy
{
public:
	EElfArcher(pi pos_);
	void update(GameState *state);
};
#endif


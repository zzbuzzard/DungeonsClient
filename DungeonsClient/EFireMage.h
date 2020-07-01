#pragma once
#include "Enemy.h"
#ifdef CLIENT
class EFireMage : public Enemy
{
public:
	EFireMage(pi pos_, ID_t id);
};
#else
class EFireMage : public Enemy
{
public:
	EFireMage(pi pos_);
	void update(GameState *state);
};
#endif


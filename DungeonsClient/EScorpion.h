#pragma once
#include "Enemy.h"

#ifdef CLIENT
class EScorpion : public Enemy
{
public:
	EScorpion(pi pos_, ID_t id);
};
#else
class EScorpion : public Enemy
{
public:
	EScorpion(pi pos_);
	void update(GameState *state);
private:
	float pauseCool;
	bool paused = false;
};
#endif


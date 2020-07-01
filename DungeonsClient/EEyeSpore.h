#pragma once
#include "Enemy.h"
#ifdef CLIENT
class EEyeSpore : public Enemy
{
public:
	EEyeSpore(pi pos_, ID_t id);
};
#else
class EEyeSpore : public Enemy
{
public:
	EEyeSpore(pi pos_);
	void update(GameState *state);

private:
	float t = 0;
};
#endif
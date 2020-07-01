#pragma once
#include "Enemy.h"
#ifdef CLIENT
class ESpider : public Enemy
{
public:
	ESpider(pi pos_, ID_t id);
};
#else
class ESpider : public Enemy
{
public:
	ESpider(pi pos_);
	void update(GameState *state);
};
#endif




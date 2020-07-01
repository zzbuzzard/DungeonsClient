#pragma once
#include "Entity.h"

class DisplayTileEntity : public Entity
{
public:
	DisplayTileEntity(pi pos_, TEXTURE texture);

	void update(GameState *state);

	bool isAlive();
	bool alive;
};


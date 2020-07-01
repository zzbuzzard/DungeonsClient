#pragma once
#include "Entity.h"
#include "Item.h"

class GameState;

class LootChest : public Entity
{
public:
	LootChest(pi pos_);
	void setRarity(ItemRarity maxRarity);

	//void pickUp(GameState *state, int i);
	void update(GameState *state);

	bool isAlive();
	bool alive = true;
};


#pragma once
#include "GameState.h"
class DungeonGameState :
	public GameState
{
// Public functions
public:
#ifdef CLIENT
	DungeonGameState(DungeonType dungeonType, sf::RenderWindow *window);
#else
	DungeonGameState(DungeonType dungeonType);
#endif
	pi getSpawnPosition() const;
	bool isAlive() const;

// Private functions
private:

// Public vars
public:
	const DungeonType dungeonType;

// Private vars
private:
};


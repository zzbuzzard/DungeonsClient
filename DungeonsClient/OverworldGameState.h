#pragma once
#include "GameState.h"
class OverworldGameState :
	public GameState
{
// Public functions
public:
#ifdef CLIENT
	OverworldGameState(WorldType worldType_, sf::RenderWindow *window);
#else
	OverworldGameState(WorldType worldType_);
#endif
	pi getSpawnPosition() const;
	bool isAlive() const;

// Private functions
private:

// Public vars
public:
	const WorldType worldType;

// Private vars
private:
};


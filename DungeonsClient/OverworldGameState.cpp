#include "pch.h"
#include "OverworldGameState.h"

// TODO: WAS THIS MEANT TO BE A SERVER ONLY CLASS? IF SO REMOVE FROM CLIENT?
//  -> IT PROBABLY SHOULDNT BE THO
#ifdef CLIENT
OverworldGameState::OverworldGameState(WorldType worldType_, sf::RenderWindow *window)
	: GameState(GameStateType::Overworld, window), worldType(worldType_)
#else
OverworldGameState::OverworldGameState(WorldType worldType_)
	: GameState(GameStateType::Overworld), worldType(worldType_)
#endif
{
	currentWorld = new World();
	currentWorld->generateOverworld(util::randint(0, 1<<31), worldType);

#ifndef CLIENT
	worldInitialised();
#endif
}

pi OverworldGameState::getSpawnPosition() const {
	return pi(util::randint(-5, 5), util::randint(-5, 5));
}

bool OverworldGameState::isAlive() const {
	return true;
}

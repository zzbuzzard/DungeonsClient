#include "pch.h"
#include "DungeonGameState.h"

#ifdef CLIENT
DungeonGameState::DungeonGameState(DungeonType dungeonType_, sf::RenderWindow *window)
	: GameState(GameStateType::Dungeon, window), dungeonType(dungeonType_)
#else
DungeonGameState::DungeonGameState(DungeonType dungeonType_)
	: GameState(GameStateType::Dungeon), dungeonType(dungeonType_)
#endif
{
	currentWorld = new World();
	currentWorld->generateDungeon(util::randint(0, 1 << 31), dungeonType);

	// TODO Wtf is this
#ifndef CLIENT
	worldInitialised();
#endif
}

pi DungeonGameState::getSpawnPosition() const {
	return pi(util::randint(-3, 3), util::randint(-3, 3));
}

bool DungeonGameState::isAlive() const {
	return true;
	//return idToPlayer.size() > 0; // TODO: Remove this line and implement the correct logic

	//if (currentWorld->bossDefeated) {
	//	if (idToPlayer.size() == 0) {
	//		return false;
	//	}
	//}
	//return true;
}

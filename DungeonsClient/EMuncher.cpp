#include "pch.h"
#include "EMuncher.h"

CombatStats muncherStats(16, 0.4f, 4, T_SPINE, 4.5f, 5);
int muncherLife = 400;
int muncherTriggerRange = 16;

#ifdef CLIENT
EMuncher::EMuncher(pi pos_, ID_t id)
	: Enemy(E_MUNCHER, pos_, muncherLife, id) {

	combatStats = muncherStats;
	setBoxSize();
	setBoxTexture(T_MUNCHER);
}
#else
LootList muncherLoot = { itemProb(I_SPEED_CHARM, 0.005f), itemProb(I_FOREST_HOOD, 0.01f) };

EMuncher::EMuncher(pi pos_)
	: Enemy(E_MUNCHER, pos_, muncherLife, muncherTriggerRange, 200) {
	combatStats = muncherStats;
	loot = &muncherLoot;
}


void EMuncher::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
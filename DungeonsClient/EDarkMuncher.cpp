#include "pch.h"
#include "EDarkMuncher.h"

static const CombatStats stats(50, 0.8f, 4, T_SHARD, 5.3f, 6);
static const int life = 1000;
static const int triggerRangee = 20;

#ifdef CLIENT
EDarkMuncher::EDarkMuncher(pi pos_, ID_t id)
	: Enemy(E_DARK_MUNCHER, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_DARK_MUNCHER);
}
#else
static const LootList Loot = { itemProb(I_CORRUPT_AMULET, 0.04f) };
static const int xp = 500;

EDarkMuncher::EDarkMuncher(pi pos_)
	: Enemy(E_DARK_MUNCHER, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
}

void EDarkMuncher::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
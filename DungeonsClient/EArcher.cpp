#include "pch.h"
#include "EArcher.h"

static const CombatStats stats(15, 0.5f, 7, T_ARROW, 2.0f, 6);
static const int life = 350;
static const int triggerRangee = 20;

#ifdef CLIENT
EArcher::EArcher(pi pos_, ID_t id)
	: Enemy(E_ARCHER, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_ARCHER);
}
#else
static const LootList Loot = { itemProb(I_BONE_BOW, 0.15f) };
static const int xp = 350;

EArcher::EArcher(pi pos_)
	: Enemy(E_ARCHER, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
}

void EArcher::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		if (targetsOutOfRange(state)) {
			moveTowardsPlayer(state);
		}
	}
	Enemy::update(state);
}
#endif
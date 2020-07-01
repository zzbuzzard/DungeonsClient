#include "pch.h"
#include "EMummyBoss.h"

const static CombatStats stats(75, 1.2f, 5, T_SAND_SPIKE, 3.8f, 17);
const static int life = 5000;
const static int triggerRangee = 30;

#ifdef CLIENT
EMummyBoss::EMummyBoss(pi pos_, ID_t id)
	: Enemy(E_MUMMY_BOSS, pos_, life, id, pi(2, 2)) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_MUMMY);
	isBoss = true;
}
#else
const static LootList Loot = { itemProb(I_DESERT_RING, 0.05f), itemProb(I_CURSED_RAGS, 0.01f) };
const static exp_t xp = 7500;

EMummyBoss::EMummyBoss(pi pos_)
	: Enemy(E_MUMMY_BOSS, pos_, life, triggerRangee, xp, pi(2, 2)) {
	combatStats = stats;
	loot = &Loot;
}

// TODO: Cooler
void EMummyBoss::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
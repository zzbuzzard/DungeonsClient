#include "pch.h"
#include "ESandGolem.h"

CombatStats sandGolemStats(30, 0.6f, 4, T_SAND_SPIKE, 2.0f, 20);
int sandGolemLife = 1000;
int sandGolemTriggerRange = 30;

#ifdef CLIENT
ESandGolem::ESandGolem(pi pos_, ID_t id)
	: Enemy(E_SAND_GOLEM, pos_, sandGolemLife, id, pi(2, 2)) {

	combatStats = sandGolemStats;
	setBoxSize();
	setBoxTexture(T_SAND_GOLEM);
}
#else
LootList sandGolemLoot = { itemProb(I_DESERT_RING, 0.005f), itemProb(I_DAMAGER, 0.03f), itemProb(I_SANDSTONE_SWORD, 0.06f), itemProb(I_RUSTY_ARMOUR, 0.2f), itemProb(I_RUSTY_HELMET, 0.3f) };

ESandGolem::ESandGolem(pi pos_)
	: Enemy(E_SAND_GOLEM, pos_, sandGolemLife, sandGolemTriggerRange, 800, pi(2, 2)) {

	combatStats = sandGolemStats;
	loot = &sandGolemLoot;
}


void ESandGolem::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
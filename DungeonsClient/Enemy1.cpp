#include "pch.h"
#include "Enemy1.h"
#include "GameState.h"
#include "Player.h"
#include "State.h"

CombatStats enemy1Stats(6, 1.2f, 5, T_FIREBALL, 2.5f, 3);
int enemy1Life = 65;
int enemy1TriggerRange = 20;

#ifdef CLIENT
Enemy1::Enemy1(pi pos_, ID_t id)
	: Enemy(E_ENEMY1, pos_, enemy1Life, id) {

	combatStats = enemy1Stats;
	setBoxSize();
	setBoxTexture(T_ENEMY1);
}
#else

LootList enemy1Loot = { itemProb(I_SAND_CAPE, 0.015f) };

Enemy1::Enemy1(pi pos_)
	: Enemy(E_ENEMY1, pos_, enemy1Life, enemy1TriggerRange, 50) {
	combatStats = enemy1Stats;
	loot = &enemy1Loot;
}


void Enemy1::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
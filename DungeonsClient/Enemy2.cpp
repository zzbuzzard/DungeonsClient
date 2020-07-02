#include "pch.h"
#include "Enemy2.h"
#include "GameState.h"
#include "Player.h"
#include "State.h"

CombatStats enemy2Stats(15, 1.2f, 6, T_FIREBALL, 2.7f, 5);
int enemy2Life = 150;
int enemy2TriggerRange = 20;

#ifdef CLIENT
Enemy2::Enemy2(pi pos_, ID_t id)
	: Enemy(E_ENEMY2, pos_, enemy2Life, id) {

	combatStats = enemy2Stats;
	setBoxSize();
	setBoxTexture(T_ENEMY2);
}
#else

LootList enemy2Loot = { itemProb(I_SAND_HELMET, 0.01f), itemProb(I_SAND_CAPE, 0.015f) };

Enemy2::Enemy2(pi pos_)
	: Enemy(E_ENEMY2, pos_, enemy2Life, enemy2TriggerRange, 150) {
	combatStats = enemy2Stats;
	loot = &enemy2Loot;
}


void Enemy2::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
#include "pch.h"
#include "EIceEnemy.h"

CombatStats iceEnemyStats(15, 0.45f, 8, T_DUST, 3.0f, 10);
int iceEnemyLife = 350;
int iceEnemyTriggerRange = 20;

#ifdef CLIENT
EIceEnemy::EIceEnemy(pi pos_, ID_t id)
	: Enemy(E_ICE_ENEMY, pos_, iceEnemyLife, id) {

	combatStats = iceEnemyStats;
	setBoxSize();
	setBoxTexture(T_ICE_ENEMY);
}
#else
LootList iceEnemyLoot = { };

EIceEnemy::EIceEnemy(pi pos_)
	: Enemy(E_ICE_ENEMY, pos_, iceEnemyLife, iceEnemyTriggerRange, 300) {
	combatStats = iceEnemyStats;
	loot = &iceEnemyLoot;
}


void EIceEnemy::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
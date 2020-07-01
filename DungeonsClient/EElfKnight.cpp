#include "pch.h"
#include "EElfKnight.h"

static const CombatStats stats(40, 1.0f, 3, T_SHARD, 4.0f, 14);
static const int life = 800;
static const int triggerRangee = 20;

#ifdef CLIENT
EElfKnight::EElfKnight(pi pos_, ID_t id)
	: Enemy(E_ELF_KNIGHT, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_ELF_KNIGHT);
}
#else
static const LootList Loot = { itemProb(I_ELVEN_BLADE, 0.04f) };
static const int xp = 500;

EElfKnight::EElfKnight(pi pos_)
	: Enemy(E_ELF_KNIGHT, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
}

void EElfKnight::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
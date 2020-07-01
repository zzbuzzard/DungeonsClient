#include "pch.h"
#include "ECursedElfKnight.h"

static const CombatStats stats(60, 0.5f, 3, T_SHARD, 7.5f, 14);
static const int life = 1350;
static const int triggerRangee = 40;

#ifdef CLIENT
ECursedElfKnight::ECursedElfKnight(pi pos_, ID_t id)
	: Enemy(E_CURSED_ELF_KNIGHT, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_CURSED_ELF_KNIGHT);
}
#else
static const LootList Loot = { itemProb(I_CURSED_ELVEN_HELMET, 0.08f), itemProb(I_ELVEN_BLADE, 0.2f) };
static const int xp = 1000;

ECursedElfKnight::ECursedElfKnight(pi pos_)
	: Enemy(E_CURSED_ELF_KNIGHT, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
}

void ECursedElfKnight::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
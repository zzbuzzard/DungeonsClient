#include "pch.h"
#include "EFireMage.h"

static const CombatStats stats(34, 2.0f, 9, T_FIREBALL, 3.0f, 5);
static const int life = 100;
static const int triggerRangee = 20;

#ifdef CLIENT
EFireMage::EFireMage(pi pos_, ID_t id)
	: Enemy(E_FIRE_MAGE, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_FIRE_MAGE);
}
#else
static const LootList Loot = { itemProb(I_MAGE_ROBE, 0.02f), itemProb(I_FIRE_STAFF, 0.035f),  itemProb(I_HOLY_ORB, 0.05f) };
static const int xp = 275;

EFireMage::EFireMage(pi pos_)
	: Enemy(E_FIRE_MAGE, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
}

void EFireMage::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		if (targetsOutOfRange(state)) {
			moveTowardsPlayer(state);
		}
	}
	Enemy::update(state);
}
#endif
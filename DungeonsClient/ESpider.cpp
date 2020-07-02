#include "pch.h"
#include "ESpider.h"

static const CombatStats stats(30, 0.8f, 3, T_SHARD, 4.5f, 10);
static const int life = 500;
static const int triggerRangee = 15;

#ifdef CLIENT
ESpider::ESpider(pi pos_, ID_t id)
	: Enemy(E_SPIDER, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_SPIDER);
}
#else
static const LootList Loot = { itemProb(I_SPIDER_FANG_DAGGER, 0.015f), itemProb(I_SILK_CAPE, 0.04f) };
static const int xp = 400;

ESpider::ESpider(pi pos_)
	: Enemy(E_SPIDER, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
}

void ESpider::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);
	}
	Enemy::update(state);
}
#endif
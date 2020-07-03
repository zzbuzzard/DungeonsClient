#include "pch.h"
#include "EScorpion.h"
#include "State.h"

static const CombatStats stats(30, 1.5f, 4, T_SPINE, 5.2f, 8);
static const int life = 200;
static const int triggerRangee = 20;

#ifdef CLIENT
EScorpion::EScorpion(pi pos_, ID_t id)
	: Enemy(E_SCORPION, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_SCORPION);
}
#else
static const LootList Loot = { itemProb(I_SCORPION_ARMOUR, 0.01f), itemProb(I_SCORPION_DAGGER, 0.15f) };
static const int xp = 325;

EScorpion::EScorpion(pi pos_)
	: Enemy(E_SCORPION, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
	pauseCool = util::rand(0, 6.0f);
}


static const float walkTime = 6.0f, pauseTime = 3.0f;
void EScorpion::update(GameState *state) {
	pauseCool -= deltaTime;
	if (pauseCool < 0) {
		if (paused) pauseCool = walkTime;
		else pauseCool = pauseTime;
		paused = !paused;
	}

	if (!paused) {
		if (currentMovement == D_NONE) {
			targetClosestPlayer(state);
			moveTowardsPlayer(state);
		}
	}
	Enemy::update(state);
}
#endif
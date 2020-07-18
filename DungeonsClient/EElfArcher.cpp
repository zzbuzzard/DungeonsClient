#include "pch.h"
#include "EElfArcher.h"

// lets say avg defence 10
// 8/0.3 = 26.6
// 15/x = 26

static const CombatStats stats(22, 0.57f, 8, T_ARROW, 5.0f, 4);
static const int life = 250;
static const int triggerRangee = 20;

#ifdef CLIENT
EElfArcher::EElfArcher(pi pos_, ID_t id)
	: Enemy(E_ELF_ARCHER, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_ELF_ARCHER);
}
#else
#include "GameState.h"
#include "Player.h"

static const LootList Loot = { itemProb(I_ELVEN_BOW, 0.05f) };
static const int xp = 450;

EElfArcher::EElfArcher(pi pos_)
	: Enemy(E_ELF_ARCHER, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
}

void EElfArcher::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		if (targetsOutOfRange(state)) {
			moveTowardsPlayer(state);
		}
		else {
			if (state->idToPlayer.find(targets[0]) != state->idToPlayer.end()) {
				pi p = state->idToPlayer[targets[0]]->getCollisionPos();
				if (util::taxicab(pos, p) <= 5) {
					fleeClosest(state);
				}
			}
		}
	}
	Enemy::update(state);
}
#endif
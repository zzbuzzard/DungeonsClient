#include "pch.h"
#include "ELeafling.h"

static const CombatStats stats(33, 0.6f, 5, T_LEAF, 2.5f, 5);
static const int life = 300;
static const int triggerRangee = 20;

#ifdef CLIENT
ELeafling::ELeafling(pi pos_, ID_t id)
	: Enemy(E_LEAFLING, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_LEAFLING);
}
#else
#include "GameState.h"

static const LootList Loot = { itemProb(I_VINE_HELMET, 0.008f), itemProb(I_FOREST_HOOD, 0.0333f) };
static const int xp = 550;

ELeafling::ELeafling(pi pos_)
	: Enemy(E_LEAFLING, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;
}

// Picks random points and moves to them when out of range
void ELeafling::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		if (targets.size() > 0 && targetsOutOfRange(state)) {
			moveTowardsPlayer(state);
		}
		// No player nearby
		// BFS to random target and walk about thru the map
		else {
			if (!tethered) {
				if (targets.size() == 0) {
					if (target == pi(0, 0) || pos == target) {
						target = pos + pi(util::randint(-20, 20), util::randint(-20, 20));
					}
					if (bfsCooldown <= 0) {
						//cout << "Leafling BFS" << endl;
						Dir d = state->BFSTowards(pos, target, 40, tileSize);
						if (d == D_NONE || !state->canMoveDir(pos, tileSize, d)) {
							target = pi(0, 0);
							bfsCooldown = 1.0f;
							//cout << "Cooldown = 1" << endl;
						}
						else {
							currentMovement = d;
							//cout << "Movement = d" << endl;
						}
					}
				}
			}
			else {
				if (tethered) {
					if (bfsCooldown <= 0) {
						Dir d = state->BFSTowards(pos, tether, 40);
						if (d == D_NONE)
							d = state->BFSTowardsNoEntities(pos, tether, 40);
						
						pi newPos = pos + dirOffset[d];
						if (d == D_NONE || !state->canMove(newPos)) {
							bfsCooldown = 0.8f;
						}
						else {
							currentMovement = d;
						}
					}
				}

				//if (follow->getLife() / (float)follow->maxLife < 0.35f) {
				//	follow = nullptr;
				//	decrementVariable = nullptr;
				//}
				//else {
				//	if (bfsCooldown <= 0) {
				//		state->BFSTowards(pos, follow->getCollisionPos(), 40);
				//	}
				//}
			}
		}
	}
	Enemy::update(state);
}
#endif
#include "pch.h"
#include "EEnt.h"

//static const CombatStats stats(35, 0.5f, 4, T_LEAF, 2.0f, 15);
static const CombatStats stats2(30, 0.18f, 4, T_FIREBALL, 3.0f, 25);
static const int life = 3500;
static const int triggerRangee = 20; // used to be 10 then set to 20 in phase 2

#ifdef CLIENT
EEnt::EEnt(pi pos_, ID_t id)
	: Enemy(E_ENT, pos_, life, id, pi(2, 2)) {

	combatStats = stats2;
	setBoxSize();
	setBoxTexture(T_ENT);
}
#else
#include "State.h"
#include "GameState.h"
//#include "ELeafling.h"

static const LootList Loot = { itemProb(I_EARTH_STAFF, 0.03f), itemProb(I_CORRUPT_AMULET, 0.035f), itemProb(I_VINE_HELMET, 0.2f) };
static const int xp = 1750;

//static const int maxLeaflings = 1;
//static const float respawnRate = 15.0f;

EEnt::EEnt(pi pos_)
	: Enemy(E_ENT, pos_, life, triggerRangee, xp, pi(2, 2)) {
	combatStats = stats2;
	loot = &Loot;

	//phase2 = false;
	//leaflingCount = std::shared_ptr<int>(new int);
	//(*leaflingCount) = 0;
	//respawnTime = 1.0f;
}

static const int phase2NumThings = 4;

void EEnt::update(GameState *state) {
	//if (!phase2) {
	//	if (lifeOn / (float)maxLife < 0.4f) {
	//		combatStats = stats2;

	//		triggerRange = 20;

	//		phase2 = true;
	//		update(state);
	//		return;
	//	}

	//	respawnTime -= deltaTime;

	//	if (currentMovement == D_NONE) {
	//		targetClosestPlayer(state);
	//		moveTowardsPlayer(state);

	//		if (respawnTime < 0 && targets.size() > 0 && *leaflingCount != maxLeaflings) {
	//			cout << "Spawning leaflings" << endl;
	//			int c = 0;
	//			while (*leaflingCount < maxLeaflings) {
	//				spawnLeafling(state);
	//				c++;
	//				if (c >= 4) break;
	//			}
	//			respawnTime = respawnRate;
	//		}
	//	}
	//}

	//else {

	if (lifeOn / (float)maxLife < 0.5f) triggerRange = 35;

		targetClosestPlayer(state);

		if (targets.size() > 0) {
			restartTime -= deltaTime;
			if (restartTime <= 0) {
				if (!making) {
					timeTilUpdate = 0;
					restartTime = 1.7f;

					for (int i = 0; i < phase2NumThings; i++) {
						lastPlaced.push_back(pos);
					}
					placedPis.insert(pos);
				}
				else {
					lastPlaced.clear();
					placedPis.clear();
					restartTime = 3.5f;
				}

				making = !making;
			}

			if (making) {
				timeTilUpdate -= deltaTime;
				if (timeTilUpdate <= 0) {
					timeTilUpdate += 0.05f;
					cout << "Ent updating the front" << endl;

					std::vector<pi> newFront;

					for (pi p : lastPlaced) {
						pi p2;
						bool w = false;
						for (int i = 0; i < 10; i++) {
							Dir d = (Dir)util::randint(0, 3);
							p2 = p + dirOffset[d];
							if (util::taxicab(p2, pos) < util::taxicab(p, pos)) {
								if (util::rand(0, 1) < 0.5f)
									continue;
							}
							if (state->currentWorld->canWalk(p2) && placedPis.find(p2) == placedPis.end()) {
								w = true;
								break;
							}
						}

						if (w) {
							newFront.push_back(p2);
							placedPis.insert(p2);

							//placedPis.insert(p2 + pi(1, 0));
							//placedPis.insert(p2 + pi(-1, 0));
							//placedPis.insert(p2 + pi(0, 1));
							//placedPis.insert(p2 + pi(0, -1));
						}
					}
					if (newFront.size() > 0) {
						state->sendTLtilesPacket(TLTileType::SPIKY_TREE, newFront);
					}
					lastPlaced = newFront;
				}
			}
			else {
				if (currentMovement == D_NONE) {
					moveTowardsPlayer(state, true);
				}
			}
		}
	//}


	Enemy::update(state);
}

//void EEnt::spawnLeafling(GameState *state) {
//	pi g;
//	bool w = false;
//	for (int i = 0; i < 10; i++) {
//		int x1 = pos.x - 1, x2 = pos.x + 2, y1 = pos.y - 1, y2 = pos.y + 2;
//
//		if (util::rand(0, 1) < 0.5f) {
//			g.x = (util::rand(0, 1) < 0.5f ? x1 : x2);
//			g.y = (util::rand(0, 1) < 0.5f ? pos.y : pos.y + 1);
//		}
//		else {
//			g.x = (util::rand(0, 1) < 0.5f ? pos.x : pos.x + 1);
//			g.y = (util::rand(0, 1) < 0.5f ? y1 : y2);
//		}
//
//		if (state->canMove(g)) {
//			w = true;
//			break;
//		}
//	}
//	if (w) {
//		(*leaflingCount)++;
//		ELeafling *e = (ELeafling*)spawnEntityID(E_LEAFLING, 0, g);
//		e->dropsLoot = false;
//		e->decrementVar = leaflingCount;
//
//		e->tethered = true;
//		e->tether = spawnP;
//
//		state->addEntity(e);
//	}
//}


#endif
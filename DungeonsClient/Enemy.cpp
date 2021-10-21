#include "pch.h"
#include "Enemy.h"

#ifdef CLIENT
Enemy::Enemy(EntitySpecies spec, pi pos_, int life, ID_t id, pi tileSize_)
	: LivingEntity(spec, ET_ENEMY, pos_, life, id, tileSize_) {
}
#else
#include "Player.h"
#include "GameState.h"
#include "State.h"
#include "Packet.h"

static const float bfsCooldownTime = 0.8f;

ID_t Enemy::idd = -1;
Enemy::Enemy(EntitySpecies spec, pi pos_, int life, int triggerRange_, exp_t exp_, pi tileSize_)
	: LivingEntity(spec, ET_ENEMY, pos_, life, idd--, tileSize_), triggerRange(triggerRange_), exp(exp_)
{	
	// check for negative overflows : hopefully enemy with id -1 is dead by now (32,000 enemies later)
	if (idd > 0) idd = -1;
	spawnP = pos;

	bfsCooldown = -1;
}

Enemy::~Enemy() {
	if (decrementVar != nullptr) {
		cout << "Enemy destructor decrementing" << endl;
		(*decrementVar)--;
	}
}

// Moves towards player
// Or returns to spawn if it cannot move towards player
// Also sets 1 target as the closest player.
void Enemy::moveTowardsPlayer(GameState *state, bool returnsToSpawn, bool ignoreOtherEntities) {
	if (currentMovement != D_NONE || bfsCooldown > 0) return;

	Player *p = state->getClosestPlayer(getCollisionPos());

	bool worked = true;
	if (p != nullptr && util::taxicabSize(pos, p->getCollisionPos(), tileSize) <= triggerRange) {
		// dont move towards if we're a boss and they're not in the boss room
		// but do shoot them to prevent them standing outside and baiting us
		if (isBoss && state->currentWorld->isBossRoom(p->getCollisionPos()) == false) {
			worked = false;
		}
		else {
			Dir d = D_NONE;

			//static const float fullLifeDepthMultiplier = 0.75f;

			//int bfsDepth = triggerRange;
			//if (lifeOn == maxLife) bfsDepth = (int)(bfsDepth * fullLifeDepthMultiplier);

			if (!ignoreOtherEntities) {
				d = state->BFSTowards(pos, p->getCollisionPos(), triggerRange, tileSize);

				if (d == D_NONE) {
					d = state->BFSTowardsRanged(pos, p->getCollisionPos(), triggerRange, combatStats.range, tileSize);
					if (d != D_NONE) {
						//cout << "An enemy is moving with BFS ranged" << endl;
					}
				}
				else {
					//cout << "An enemy is moving normally" << endl;
				}
			}

			if (d == D_NONE) {
				d = state->BFSTowardsNoEntities(pos, p->getCollisionPos(), triggerRange, tileSize);
			}

			if (d == D_NONE) {
				// BFS couldn't find a way; targets are out of range. Give up, return to spawn
				if (targetsOutOfRange(state)) {
					worked = false;

					// We will proceed to move to spawn
					if (returnsToSpawn)
						bfsCooldown = -1;
					else
						bfsCooldown = bfsCooldownTime;
				}
				else {
					// BFS couldn't find a way but we're in range, we could be right next to em
					bfsCooldown = bfsCooldownTime * 0.5f;
				}
			}

			else {
				if (tileSize == pi(1, 1)) {
					pi newPos = pos + dirOffset[d];
					if (state->canMove(newPos)) {
						currentMovement = d;
					}
					else {
						bfsCooldown = bfsCooldownTime;
					}
				}
				else {
					if (state->canMoveDir(pos, tileSize, d)) {
						currentMovement = d;
					}
					else {
						bfsCooldown = bfsCooldownTime;
					}
				}
			}
		}
	}
	else
		worked = false;

	// Return to spawn if we're not attacking player and unable to reach them
	if (returnsToSpawn && !worked) moveToSpawn(state);
}


void Enemy::moveToSpawn(GameState *state) {
	if (pos != spawnP && bfsCooldown <= 0) {
		Dir d = state->BFSTowards(pos, spawnP, 40, tileSize);
		if (d == D_NONE) {
			bfsCooldown = 3 * bfsCooldownTime;
		}
		else {
			if (tileSize == pi(1, 1)) {
				pi newPos = pos + dirOffset[d];
				if (state->canMove(newPos)) {
					currentMovement = d;
				}
				else {
					// Should never happen...?
					bfsCooldown = 2 * bfsCooldownTime;
				}
			}
			else {
				if (state->canMoveDir(pos, tileSize, d)) {
					currentMovement = d;
				}
				else {
					// Should never happen...?
					bfsCooldown = 2 * bfsCooldownTime;
				}
			}
		}
	}
}


void Enemy::fleeClosest(GameState *state) {
	if (bfsCooldown > 0) return;
	if (currentMovement != D_NONE) return;

	Player *p = state->getClosestPlayer(getCollisionPos());
	if (p == nullptr) return;

	pi playerPos = p->getCollisionPos();
	Dir d = state->BFSTowards(pos, playerPos, triggerRange, tileSize);

	if (d == D_NONE) {
		bfsCooldown = bfsCooldownTime;
		return;
	}

	Dir d2 = (Dir)(((int)d + 2) % 4);
	pi pos2 = pos + dirOffset[d2];
	if (state->canMove(pos2, tileSize)) {
		currentMovement = d2;
		return;
	}

	d2 = (Dir)((d+1)%4);
	pos2 = pos + dirOffset[d2];
	if (state->canMove(pos2, tileSize)) {
		currentMovement = d2;
		return;
	}

	d2 = (Dir)((d+3) % 4);
	pos2 = pos + dirOffset[d2];
	if (state->canMove(pos2, tileSize)) {
		currentMovement = d2;
		return;
	}

	//cout << "Exhausted all possibilities" << endl;
}

bool Enemy::targetsOutOfRange(GameState *state) {
	for (ID_t id : targets) {
		if (state->idToPlayer.find(id) != state->idToPlayer.end()) {
			Player *p = state->idToPlayer[id];

			// If any are in range, return false
			if (util::taxicabSize(getCollisionPos(), p->getCollisionPos(), tileSize) <= combatStats.range)
				return false;
		}
	}
	return true;
}

const static float globalRetargetCooldown = 0.2f;

void Enemy::targetClosestPlayer(GameState *state) {
	if (retargetCooldown > 0) return;
	retargetCooldown = globalRetargetCooldown;

	targets.clear();

	Player *p = state->getClosestPlayer(getCollisionPos());

	if (p != nullptr && util::taxicabSize(getCollisionPos(), p->getCollisionPos(), tileSize) <= triggerRange)
		targets.push_back(p->getID());
}

void Enemy::targetClosestPlayers(GameState *state, int k) {
	if (retargetCooldown > 0) return;
	retargetCooldown = globalRetargetCooldown;

	auto vec = state->getClosestPlayers(pos, k);
	targets.clear();
	for (Player *p : vec) {
		if (p != nullptr && util::taxicabSize(getCollisionPos(), p->getCollisionPos(), tileSize) <= triggerRange)
			targets.push_back(p->getID());
	}
}


void Enemy::update(GameState *state) {
	LivingEntity::update(state);
	bfsCooldown -= deltaTime;
	retargetCooldown -= deltaTime;
}

void Enemy::takeDamage(int dmg, ID_t damageDealer) {
	int L = lifeOn;
	LivingEntity::takeDamage(dmg, damageDealer);
	int actualDamageDealt = L - std::max(0, lifeOn); // this method accounts for defence status effects etc. 

	// Players only, but if enemies deal damage we will need to also count
	//  total player damage dealt (unlikely anyway, would be a super weird mechanic)
	if (actualDamageDealt > 0 && damageDealer >= 0) {
		//cout << "Damage dealt: " << actualDamageDealt << endl;
		if (damageMap.find(damageDealer) == damageMap.end()) damageMap[damageDealer] = actualDamageDealt;
		else damageMap[damageDealer] += actualDamageDealt;
	}
}

// Get the % of total XP we get, if we did p% of the damage.
// Should be generous to promote teamwork.
float getXpMul(float p) {
	if (p >= 0.6f) return 1.0f;
	if (p >= 0.4f) return 0.8f;
	if (p >= 0.25f) return 0.4f;
	if (p >= 0.1f) return 0.25f;
	if (p >= 0.05f) return 0.1f;
	if (p >= 0.01f) return 0.05f;
	return 0.0f;
}

// 5% damage for (low) loot chance
#define MIN_AMOUNT_FOR_LOOT 0.05

void Enemy::onDie(GameState *state) {
	// LootList was never set: NOTE THIS SHOULD NEVER HAPPEN (NO XP)
	if (loot == nullptr) return;
	// Drops no loot/XP e.g. boss battle minion
	if (!dropsLoot) return;

	cout << "Enemy ID " << ID << " has died" << endl;
	for (auto it = damageMap.begin(); it != damageMap.end(); ++it) {
		ID_t id = it->first;
		float mul = it->second / (float)maxLife;
		std::vector<ITEM> theirItems;

		cout << "Player ID " << id << " dealt " << util::floatToString(100 * mul, 1) << "% of the damage" << endl;

		// If player doesnt exist or insufficient damage for loot, continue
		if (state->idToPlayer.find(id) == state->idToPlayer.end()) continue;
		if (mul >= MIN_AMOUNT_FOR_LOOT) {
			for (const itemProb i : *loot) {
				if (util::rand(0, 1) <= i.second * mul) {
					cout << "They got a " << itemTypes[i.first]->name << endl;
					theirItems.push_back(i.first);
				}
			}
		}

		Player *p = state->idToPlayer[id];

		float xpMul = getXpMul(mul);
		exp_t gain = (exp_t)(exp * xpMul);

		//cout << "They get +" << gain << "XP" << endl;
		p->pInfo.gainXP(gain);
		if (gain > 0)
			p->pInfo.saveXP(p->ID);

		if (theirItems.size() > 0) {
			p->pInfo.createLoot(getCollisionPos(), theirItems);
		}

		sf::Packet packet;
		packet.append(&P_LOOT, sizeof(Packet));
		pos_t x = getCollisionPos().x,
			y = getCollisionPos().y;

		packet.append(&gain, sizeof(exp_t));

		packet.append(&x, sizeof(pos_t));
		packet.append(&y, sizeof(pos_t));

		item_num_t numItems = (item_num_t)theirItems.size();
		packet.append(&numItems, sizeof(item_num_t));
		for (ITEM i : theirItems) packet.append(&i, sizeof(ITEM));

		p->sendPacket(packet);
	}
}

void Enemy::setTriggerRange(int t) {
	triggerRange = t;
}


#endif
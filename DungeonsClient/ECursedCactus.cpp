#include "pch.h"
#include "ECursedCactus.h"

// Phase 1: idea is you must keep away from the boss while also tackling scorpions
//  Stand in center, and slowly move towards player
//  Attack with low range, big damage
//  Attack up to 4
//  Spawn in scorpions from the edges

// Phase 2: 
//  Move quickly towards players
//  Quick high damage shots


// 100 / 1.5
// 30 / 0.2

static const CombatStats p1_stats(35, 0.4f, 5, T_FIREBALL, 2.4f, 15);
static const CombatStats p2_stats(40, 0.2f, 3, T_THORN, 5.0f, 15);
static const int life = 8500;
static const int triggerRangee = 40;

static const float spawnTime = 1.0f;
static const float spawningTime = 10.0f;
static const float notSpawningTime = 32.5f;

static const float p2_notMovingTime = 5.0f, p2_movingTime = 10.0f;
static const std::vector<EntitySpecies> spawnList = { E_SCORPION };

#ifdef CLIENT
ECursedCactus::ECursedCactus(pi pos_, ID_t id)
	: Enemy(E_CURSED_CACTUS, pos_, life, id, pi(3, 3)) {

	combatStats = p1_stats;
	setBoxSize();
	setBoxTexture(T_CURSED_CACTUS);
	isBoss = true;
}
#else
#include "GameState.h"
#include "State.h"

static const LootList Loot = { itemProb(I_CURSED_CACTUS_EYE, 0.05f), itemProb(I_CACTUS_SWORD, 0.175f) };
static const int xp = 10000;

ECursedCactus::ECursedCactus(pi pos_)
	: Enemy(E_CURSED_CACTUS, pos_, life, triggerRangee, xp, pi(3, 3)) {
	combatStats = p1_stats;
	loot = &Loot;

	isSpawning = true;
	p1_isSpawningCool = spawningTime;
}

void ECursedCactus::update(GameState *state) {
	if (PHASE == 0) {
		PHASE = 1;
		xRange = pi(state->currentWorld->bossRoomBL.x, state->currentWorld->bossRoomTR.x);
		yRange = pi(state->currentWorld->bossRoomBL.y, state->currentWorld->bossRoomTR.y);
	}


	if (PHASE == 1) {
		if (lifeOn / (float)maxLife < 0.5f) {
			PHASE = 2;
			
			combatStats = p2_stats;
			p2_isMoving = false;
			p2_moveTime = p2_notMovingTime;

			update(state);
			return;
		}
		if (currentMovement == D_NONE) {
			targetClosestPlayer(state);
			moveTowardsPlayer(state);
		}
		p1_isSpawningCool -= deltaTime;
		if (p1_isSpawningCool <= 0) {
			if (isSpawning)
				p1_isSpawningCool = notSpawningTime;
			else
				p1_isSpawningCool = spawningTime;
			isSpawning = !isSpawning;
		}

		if (isSpawning) {
			p1_spawnCool -= deltaTime;
			while (p1_spawnCool <= 0) {
				p1_spawnCool += spawnTime;

				EntitySpecies spec = spawnList[util::randint(0, spawnList.size() - 1)];

				for (int i = 0; i < 50; i++) {
					pi pos = util::randomBoxBorderPoint(xRange.x, xRange.y, yRange.x, yRange.y);
					if (state->canMove(pos)) {
						Enemy *e = (Enemy*)spawnEntityID(spec, 0, pos);
						e->dropsLoot = false;
						state->addEntity(e);
						break;
					}
				}
			}
		}
	}
	if (PHASE == 2) {
		p2_moveTime -= deltaTime;
		if (p2_moveTime <= 0) {
			if (p2_isMoving) {
				p2_moveTime = p2_notMovingTime;

				auto vec = util::borderPoints(getCollisionPos(), tileSize, 1, 3);
				state->sendTLtilesPacket(TLTileType::SPIKE_FLOOR, vec);
			}
			else
				p2_moveTime = p2_movingTime;
			p2_isMoving = !p2_isMoving;
		}


		if (currentMovement == D_NONE) {
			targetClosestPlayers(state, 4);
			if (p2_isMoving)
				moveTowardsPlayer(state);
		}
	}
	Enemy::update(state);
}
#endif
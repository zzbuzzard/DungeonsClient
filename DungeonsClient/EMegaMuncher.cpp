#include "pch.h"
#include "EMegaMuncher.h"

static const CombatStats p1_stats(45, 0.25f, 2, T_SPINE, 3.9f, 18);
static const CombatStats p2_stats(45, 0.5f, 2, T_THORN, 5.5f, 18);
static const int life = 15000;
static const int triggerRangee = 100;

#ifdef CLIENT
EMegaMuncher::EMegaMuncher(pi pos_, ID_t id)
	: Enemy(E_MEGA_MUNCHER, pos_, life, id, pi(2, 2)) {

	combatStats = p1_stats;
	setBoxSize();
	setBoxTexture(T_MEGA_MUNCHER);
	isBoss = true;
}
#else
#include "GameState.h"
#include "State.h"

static const LootList Loot = { };
static const int xp = 15000;

EMegaMuncher::EMegaMuncher(pi pos_)
	: Enemy(E_MEGA_MUNCHER, pos_, life, triggerRangee, xp, pi(2, 2)) {
	combatStats = p1_stats;
	loot = &Loot;

	PHASE = 1;
	p1_changeTime = 0;
	p1_isChasing = false;
}

// Map is a thick 2x2 maze with no dead ends (used the algorithm I made for tings pacman)
// Phase 1: Chase the player through the maze like a normal muncher.
// Phase 2: Stop and shoot out a wave of things that pass through the maze.
//          WaveFront has a min size and more are spawned when they die
// Alternate between phase 1 and 2 with time
// Phase 3: Chase player quickly through the maze
//          Pause to spawn munchers
//          Up to a max of like 10

//float p1_changeTime;
//bool p1_isChasing = true;
//std::vector<pi> p1_front;



static const float p1_chaseTime = 12.0f, p1_treeTime = 10.0f, p1_moveFrontRate = 0.175f,
	p2_chaseTime = 15.0f, p2_spawnTime = 5.0f, p2_spawnRate = 0.5f;
static const int p1_minTrees = 10, p2_maxMunchers = 10;

void EMegaMuncher::update(GameState *state) {
	if (PHASE == 1) {
		if (lifeOn / (float)maxLife < 0.35f) {
			PHASE = 2;
			update(state);

			p2_muncherCount = std::shared_ptr<int>(new int);
			*p2_muncherCount = 0;
			
			p2_isChasing = true;
			p2_changeTime = 0;
			
			combatStats = p2_stats;

			return;
		}

		p1_changeTime -= deltaTime;
		
		if (p1_changeTime <= 0) {
			p1_isChasing = !p1_isChasing;
			if (p1_isChasing) {
				p1_changeTime = p1_chaseTime;
				p1_front.clear();
			}
			else {
				p1_changeTime = p1_treeTime;
			}
		}

		if (p1_isChasing) {
			if (currentMovement == D_NONE) {
				targetClosestPlayers(state, 3);
				moveTowardsPlayer(state, true);
			}
		}
		else {
			targets.clear();
			p1_moveFrontTime -= deltaTime;
			if (p1_moveFrontTime <= 0) {
				p1_moveFrontTime += p1_moveFrontRate;

				std::vector<pi> news;

				// Move front
				for (pi p : p1_front) {
					pi p2;
					for (int i = 0; i < 4; i++) {
						p2 = p + dirOffset[i];
						if (state->currentWorld->canWalk(p2)) {
							news.push_back(p2);
						}
					}
				}

				// Spawn new one
				if (p1_front.size() < p1_minTrees) {
					if (state->currentWorld->canWalk(pos)) {
						news.push_back(pos);
					}
				}

				p1_front = state->sendTLtilesPacket(TLTileType::SPIKY_TREE, news);
			}
		}
	}

	//std::shared_ptr<int> p2_muncherCount;
	//float p2_changeTime;
	//bool p2_isChasing ;
	//float p2_spawnCool = 0;

	if (PHASE == 2) {
		p2_changeTime -= deltaTime;
		if (p2_changeTime <= 0) {
			p2_isChasing = !p2_isChasing;
			if (p2_isChasing) {
				p2_changeTime = p2_chaseTime;
			}
			else {
				p2_changeTime = p2_spawnTime;
				p2_spawnCool = 0;
				targets.clear();
			}
		}

		if (p2_isChasing) {
			if (currentMovement == D_NONE) {
				targetClosestPlayers(state, 3);
				moveTowardsPlayer(state, true, true);
			}
		}
		else {
			if (*p2_muncherCount < p2_maxMunchers) {
				p2_spawnCool -= deltaTime;
				if (p2_spawnCool <= 0) {
					p2_spawnCool += p2_spawnRate;

					auto pis = util::borderPoints(getCollisionPos(), tileSize, 1, 1);

					for (int i = 0; i < 10; i++) {
						pi p = pis[util::randint(0, pis.size() - 1)];
						if (state->canMove(p)) {

							Enemy *e = (Enemy*)spawnEntityID(E_MUNCHER, 0, p);
							e->decrementVar = p2_muncherCount;
							(*p2_muncherCount)++;
							e->dropsLoot = false;
							e->setTriggerRange(100);
							e->combatStats.spd = 5.5f;
							state->addEntity(e);

							break;
						}
					}
				}
			}
		}
	}

	Enemy::update(state);
}
#endif





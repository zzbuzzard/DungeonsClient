#include "pch.h"
#include "LivingEntity.h"
#include "State.h"
#include "GameState.h"
#include "Player.h"
#include "Projectile.h"
#include "Enemy.h"

sf::Color lifebarMaxCol(0, 255, 0), lifebarMinCol(255, 0, 0), lifebarMedCol = sf::Color::Yellow;

LivingEntity::LivingEntity(EntitySpecies spec, EntityType type, pi pos_, life_t life, ID_t id, pi tileSize_) :
	Entity(spec, type, pos_, tileSize_), ID(id)
{
	maxLife = life;
	lifeOn = maxLife;
	currentMovement = D_NONE;

#ifdef CLIENT
	resizeLifebar();
	lifebarbox.setFillColor(sf::Color::Black);

	int k = std::max(tileSize.x, tileSize.y);
	targetCircle.setSize(pf(PIXEL_PER_UNIT, PIXEL_PER_UNIT) * 1.2f * (float)k);
	targetCircle.setOrigin(targetCircle.getSize() * 0.5f);
	targetCircle.setTexture(&textures[T_CIRCLE]);
	targetCircle.setFillColor(sf::Color::Red);

	specialTargetCircle.setSize(pf(PIXEL_PER_UNIT, PIXEL_PER_UNIT) * 1.3f * (float)k);
	specialTargetCircle.setOrigin(specialTargetCircle.getSize() * 0.5f);
	specialTargetCircle.setTexture(&textures[T_CIRCLE]);
	specialTargetCircle.setFillColor(sf::Color::Blue);
#endif
}

#ifdef CLIENT

// Syntax:
//  If player:
//   ID
//  If enemy:
//   ID
//   Species
//  X (short)       2 bytes
//  Y (short)       2 bytes
//  Life (int)      4 bytes
//  MoveDir (Dir)   1 byte
//  Move completion (float) 4 bytes -> is this needed?
//  CombatStats
//  Targets [count (char) then ID_ts]

// NEW SYNTAX:
//  ID + Species handled outside of function ->
//  (X, Y) : Short
//  (PROC) : int8_t
//  (Life) : int
//  (stats) : CombatStats
//  Targets : [int8_t -> ID_ts]
//  Moves processed number : int8_t


// If we just send positions out,
// E.g. I am at this tile, I am at this tile,
//      NOW I'm at this tile (start animation) then that works fine...
//      Animation duration : Speed.
//      That's their new pos ; move towards it with their speed
//      However, if newest pos != current pos, move at high speed


#include "Controller.h"

// This function gets all the data from an update for this thing.
// It assigns them all directly, unless we are the local player in which case we need to check our position is right
const void *LivingEntity::handleUpdate(const void *data) {
	// set life immediately
	pos_t *Pos_t = (pos_t*)data;
	pos_t x = *Pos_t;
	Pos_t++;
	pos_t y = *Pos_t;
	Pos_t++;

	//cout << "Handling update; my position = " << x << ", " << y << endl;
	life_t *Life = (life_t*)Pos_t;
	lifeOn = *Life;
	Life++;

	if (type == ET_PLAYER) {
		if (!local || (g_gameState && ((GameState*)g_state)->specialUsedTime == -1))
			((Player*)this)->mpOn = *(mp_t*)Life;
		Life = (life_t*)((mp_t*)Life + 1); // yeugh

		// We may have not received maxLife from server yet,
		// So this will look the most natural (better than lifebar being realllyyy long)
		if (!local) {
			if (lifeOn > maxLife) maxLife = lifeOn;
			Player *p = (Player*)this;
			if (p->mpOn > p->maxMP) p->maxMP = p->mpOn;
		}
	}

	// Removed dir, moved, etc. 
	//Dir *Dire = (Dir*)Int;
	//Dir dir = *Dire;
	//Dire++;
	//float *Float = (float*)Dire;
	//float movedd = *Float;
	//Float++;

	target_count_t *Target_count;

	if (!speciesCombatStatsConstant[(int)spec]) {
		CombatStats *CombatStatss = (CombatStats*)Life;
		combatStats = *CombatStatss;
		CombatStatss++;
		Target_count = (target_count_t*)CombatStatss;
	}
	else
		Target_count = (target_count_t*)Life;

	target_count_t n = *Target_count;
	Target_count++;
	ID_t *ID_p = (ID_t*)Target_count;
	if (!local) targets.clear();
	for (auto i = 0; i < n; i++) {
		if (!local) targets.push_back(*ID_p);
		//cout << "Targeting someone with id = " << *ID_p << endl;
		ID_p++;
	}

	if (!local) {
		if (!hasMovedBefore) {
			hasMovedBefore = true;
			pos = pi(x, y);
		}

		positions.push_back(pi(x, y));

		// Lerp system
		//Move m = Move(pi(x, y), dir, movedd);
		//positions.push_back(m);
	}
	else {
		((GameState*)g_state)->handleMyPos(pi(x, y));
		//if (currentMovement == D_NONE && dir == D_NONE) pos = pi(x, y);
	}

	//Move mov(pi(x, y), dir, moved);
	//positions.push_back(mov);

	return ID_p;
}


void LivingEntity::draw(sf::RenderWindow *window, GameState *state) {
	if (isSpecialTargetted) {
		specialTargetCircle.setPosition(getPosWorldCentered());
		window->draw(specialTargetCircle);
	}

	if (isTargetted) {
		targetCircle.setPosition(getPosWorldCentered());
		window->draw(targetCircle);
	}

	window->draw(lifebarbox);
	window->draw(lifebar);
	Entity::draw(window, state);

	//if (positions.size() > 0) {
	//	sf::RectangleShape x;
	//	x.setSize(sf::Vector2f(20, 20));
	//	x.setOrigin(x.getSize()*0.5f);
	//	x.setFillColor(sf::Color::Red);
	//	Move m = positions.back();
	//	sf::Vector2f t = (sf::Vector2f)(m.pos * PIXEL_PER_UNIT);
	//	t += PIXEL_PER_UNIT * m.moved * (sf::Vector2f)dirOffset[m.dir];
	//	x.setPosition(t);
	//	window->draw(x);
	//}
}



void LivingEntity::resizeLifebar() {
	float w = lifeOn / (float)maxLife;

	if (w <= 0.5f)
		lifebar.setFillColor(util::colorLerp(lifebarMinCol, lifebarMedCol, w*2));
	else
		lifebar.setFillColor(util::colorLerp(lifebarMedCol, lifebarMaxCol, w * 2 - 1));

	lifebarbox.setSize(sf::Vector2f(lifeBarWidth * tileSize.x, lifeBarHeight));
	//lifebarbox.setOrigin(lifebarbox.getSize()*0.5f);
	lifebarbox.setOrigin(sf::Vector2f(0, lifebar.getSize().y*0.5f));
	lifebar.setSize(sf::Vector2f(lifeBarWidth * tileSize.x * lifeOn / maxLife, lifeBarHeight));
	lifebar.setOrigin(sf::Vector2f(0, lifebar.getSize().y*0.5f));
}


#endif

void LivingEntity::setLife(life_t life) {
	lifeOn = life;
}

#include <iostream>
using std::cout; using std::endl;

void LivingEntity::moveNormally() {
	moved += combatStats.spd * deltaTime *
// (I hope God forgives me for this atrocity)
#ifdef CLIENT
	(local ? 0.95f : 1.0f);
#else
	1.0f;
#endif
	if (moved >= 1) {
		//cout << "Moved to next tile" << endl;
		pos += dirOffset[currentMovement];
		moved = 0;
		currentMovement = D_NONE;
	}
}

//const float lifeBarDisplacementMultiplier = 0.6f;
const float lifeBarYGap = 10.0f,
			lifeBarYGapPlayer = 30.0f;

void LivingEntity::update(GameState *state) {
#ifdef CLIENT
	isTargetted = false;
	isSpecialTargetted = false;

	if (local) {
		if (currentMovement != D_NONE) {
			moveNormally();
		}
	}
	else {
		// Update: now that positions is only pis,
		// We just animate normally with spd towards the first in queue.
		// However, if the queue size gets too large, speed gets multiplied.

		// ideally its always 1: just as we reach it, the next comes in.
		if (positions.size() > 0) {
			//Move m = positions.front();
			pi newPos = positions.front();

			float mul = 1.1f;
			if (positions.size() > 1) mul = 1.5f;
			if (positions.size() > 2) mul = 2.0f;
			if (positions.size() > 3) mul = 4.0f;
			if (positions.size() > 5) mul = 6.0f;
			if (positions.size() > 10) mul = 8.0f;

			if (currentMovement == D_NONE) {
				positions.pop_front();
				if (newPos == getCollisionPos()) {
				}
				else {
					if (util::taxicab(pos, newPos) > 5) {
						pos = newPos;
					}
					else {
						currentMovement = util::getDirectionTowards(pos, newPos);
						moved = 0;

						float orig = combatStats.spd;
						combatStats.spd *= mul;
						moveNormally();
						combatStats.spd = orig;
					}
				}
			}
			else {
				float orig = combatStats.spd;
				combatStats.spd *= mul;
				moveNormally();
				combatStats.spd = orig;
			}
		}
	}
#else
	if (currentMovement != D_NONE) {
		moveNormally();
	}
#endif

	if (reloadOn > 0)
		reloadOn -= deltaTime;
	if (reloadOn <= 0 && targets.size() > 0) {
		reloadOn += combatStats.reloadTime;
		for (int i = 0; i < targets.size(); i++) {
			ID_t t_id = targets[i];
			if (t_id < 0) // target enemy
			{
				if (state->idToEnemy.find(t_id) == state->idToEnemy.end()) {
					targets.erase(targets.begin() + i);
					i--;
					continue;
				}
				Enemy *enemy = state->idToEnemy[t_id];

				// Skip this target if they're a boss and we're tryna snipe them from outside the boss room
				if (enemy->isBoss) {
					if (!state->currentWorld->isBossRoom(getCollisionPos()))
						continue;
				}
				// Use their tileSize in the taxicab because we are a player so have tilesize(1, 1)
#ifdef CLIENT
				float range = combatStats.range;
				//if (currentMovement != D_NONE) range += combatStats.spd * 0.2f;
				if (util::taxicabSize(enemy->getCollisionPos(), getCollisionPos(), enemy->tileSize) <= range) {
					state->addEntity(new Projectile(combatStats.t, getPosWorldCentered(), enemy, combatStats.damage));
				}
#else
				if (util::taxicabSize(enemy->getCollisionPos(), getCollisionPos(), enemy->tileSize) <= combatStats.range) {
					enemy->takeDamage(combatStats.damage, ID);
				}
#endif
			}
			else { // target player
				if (state->idToPlayer.find(t_id) == state->idToPlayer.end()) {
					targets.erase(targets.begin() + i);
					i--;
					continue;
				}

				Player *p = state->idToPlayer[t_id];
#ifdef CLIENT
				float range = combatStats.range;
				//if (currentMovement != D_NONE) range += combatStats.spd * 0.2f;
				if (util::taxicabSize(getCollisionPos(), p->getCollisionPos(), tileSize) <= range) {
					state->addEntity(new Projectile(combatStats.t, getPosWorldCentered(), p, combatStats.damage));
				}
#else
				if (util::taxicabSize(getCollisionPos(), p->getCollisionPos(), tileSize) <= combatStats.range) {
					p->takeDamage(combatStats.damage, ID);
				}
#endif
			}
		}
	}

#ifdef CLIENT
	static const float clickMultiplier = 3.0f;

	if (mouseClicked || rightMouseClicked) {
		sf::Vector2f mouse = state->getMousePosWorld();
		sf::Vector2f center = getPosWorldCentered();
		sf::Vector2f size = sf::Vector2f(PIXEL_PER_UNIT * tileSize.x, PIXEL_PER_UNIT * tileSize.y);

		//if (tileSize != pi(1, 1)) {
		//	center -= sf::Vector2f(PIXEL_PER_UNIT, PIXEL_PER_UNIT) * 0.5f;
		//	center += size * 0.5f;
		//}

		size *= clickMultiplier;

		if (mouse.x > center.x - size.x * 0.5f && mouse.x < center.x + size.x * 0.5f) {
			if (mouse.y > center.y - size.y * 0.5f && mouse.y < center.y + size.y * 0.5f) {
				Entity** var = &(mouseClicked ? state->clickedObj : state->rightClickedObj);

				if (*var != nullptr) {
					float theirDist = util::dist((*var)->getPosWorldCentered(), mouse);
					float myDist = util::dist(getPosWorldCentered(), mouse);

					if (myDist < theirDist)
						*var = this;
				}
				else {
					*var = this;
				}
			}
		}
	}


	resizeLifebar();

	float overlap = (lifeBarWidth - PIXEL_PER_UNIT) * tileSize.x;

	float x = - (PIXEL_PER_UNIT + overlap) / 2.0f;
	float y = (tileSize.y - 0.5f) * PIXEL_PER_UNIT + (type == ET_PLAYER ? lifeBarYGapPlayer : lifeBarYGap);

	lifebarbox.setPosition(getPosWorld() + sf::Vector2f(x, y));
	lifebar.setPosition(getPosWorld() + sf::Vector2f(x, y));
#endif
}

void LivingEntity::handleTarget(ID_t id) {
	bool heal = combatStats.damage < 0;

	if (type == ET_PLAYER) {
		if (heal  && id < 0) return;  //healing an enemy
		if (!heal && id >= 0) return; //attacking a friend
	}

	for (int i = 0; i < targets.size(); i++) {
		if (targets[i] == id) {
			targets.erase(targets.begin() + i);
			return;
		}
	}
	targets.push_back(id);
}

const life_t &LivingEntity::getLife() {
	return lifeOn;
}

#define MIN_DEALT_DAMAGE_MUL 0.1f

damage_t LivingEntity::getDamageOf(damage_t dmg) {
	if (dmg < 0) return dmg; // no def applied to heals

	damage_t x = dmg - combatStats.def;

	damage_t min = MIN_DEALT_DAMAGE_MUL * dmg;
	if (min < 1) min = 1;

	if (x < min) x = min;
	return x;
}

const pf LivingEntity::getPosWorld() const {
	if (currentMovement != D_NONE)
		return (float)PIXEL_PER_UNIT*((pf)pos + (pf)dirOffset[currentMovement] * moved);
	return (float)PIXEL_PER_UNIT*(pf)pos;
}

pi LivingEntity::getCollisionPos() const {
#ifdef CLIENT
	if (positions.size() > 0) {
		return positions.front();
		//Move m = positions.front();
		//if (m.dir == D_NONE) return m.pos;
		//return m.pos + dirOffset[m.dir];
	}
#endif

	if (currentMovement == D_NONE)
		return pos;
	return pos + dirOffset[currentMovement];
}

bool LivingEntity::isAlive() {
#ifdef CLIENT
	return exists;
#else
	return lifeOn > 0;
#endif
}

void LivingEntity::takeDamage(damage_t dmg, ID_t damageDealer) {
	lifeOn -= getDamageOf(dmg);
	if (lifeOn > maxLife) lifeOn = maxLife;
}

void LivingEntity::takeDamagePierce(damage_t dmg, ID_t damageDealer) {
	lifeOn -= dmg;
	if (lifeOn > maxLife) lifeOn = maxLife;
}


ID_t LivingEntity::getID() {
	return ID;
}

void LivingEntity::handleDirectionInput(Dir d, GameState *state) {
	if (currentMovement == D_NONE) {
		pi newPos = getPos() + dirOffset[d];
		if (state->canMove(newPos)) {
			currentMovement = d;
		}
	}
}

void LivingEntity::popTargets(target_count_t maxTargets) {
	if (maxTargets < 0) maxTargets = 0;
	while (targets.size() > maxTargets) {
		targets.erase(targets.begin());
	}
}

#include "pch.h"
#include "Projectile.h"
#include "State.h"
#include "GameState.h"
#include "LivingEntity.h"
#include "DamageText.h"
#include "Player.h"

float projSpd = 1600.0f;

sf::Vector2f projSize(55.0f, 55.0f);

Projectile::Projectile(TEXTURE texture_, sf::Vector2f start, LivingEntity *target_, int damage)
	: Entity(E_NONE, ET_PROJECTILE, pi(0, 0)), target(target_), pos(start), projectileDamage(damage)
{
	setBoxSize(projSize); // todo : variable
	setBoxTexture(texture_);
	targetID = target->ID;
	canWalk = true;
}

void Projectile::draw(sf::RenderWindow *window, GameState *state) {
	sf::Transform t;
	sf::Vector2f p = getPosWorld();
	t.rotate(angleInDegrees, p);
	box.setPosition(p);
	window->draw(box, t);
}

// Every projectile calls a map.find operation 60 times a second.
// This could be massively sped up using a shared_ptr for LivingEntity, or something...
//  or store projectiles in a map then notify them (set target=nullptr) when something is removed from idToEnemy/idToPlayer
// PROFILE THIS FIRST THOUGH: PROJECTILES PROBABLY NEGLIGIBLE % OF WORK DONE PER SEC
void Projectile::update(GameState *state) {
	if (!alive) return;

	// once its nullptr we move towards targetPos
	if (target != nullptr) {
		if ((targetID < 0 && state->idToEnemy.find(targetID) == state->idToEnemy.end()) ||
			targetID >= 0 && state->idToPlayer.find(targetID) == state->idToPlayer.end()) {
			target = nullptr;
		}
	}

	if (target != nullptr)
		targetPos = target->getPosWorldCentered();

	if (!state->isInView((pi)(targetPos / (float)PIXEL_PER_UNIT))) {
		alive = false;
		return;
	}

	sf::Vector2f disp = targetPos - pos;
	float ang = util::ang(disp);
	angleInDegrees = 180/PI * ang;

	float mag = util::mag(disp);
	pos += disp / mag * projSpd * deltaTime;

	if (mag <= PIXEL_PER_UNIT) {
		alive = false;

		if (target != nullptr) {
			int actualDamage = target->getDamageOf(projectileDamage);
			float yOffset = -target->tileSize.y * 0.5f * (float)PIXEL_PER_UNIT;

			state->addEntity(new DamageText(target, pf(0, yOffset), actualDamage));
		}
		else {
			cout << "Target is nullptr" << endl;
		}
	}
}

bool Projectile::isAlive() {
	return alive;
}

const sf::Vector2f Projectile::getPosWorld() const {
	// t = 0 : end
	// t = distance : start
	return pos;
	//return endF - t * perSec;
}

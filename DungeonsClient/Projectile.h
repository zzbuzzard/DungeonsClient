#pragma once
#include "Entity.h"
#include "Media.h"

class LivingEntity;

// All projectiles are the same; we only need 1 class
// It has 1) a texture
//        2) a bullet speed
//        3) a target tile

class Projectile : public Entity
{
public:
	Projectile(TEXTURE texture_, sf::Vector2f start, LivingEntity *target, int damage);
	void draw(sf::RenderWindow *window, GameState *state);
	void update(GameState *state);
	bool isAlive();
	const sf::Vector2f getPosWorld() const;

private:
	//float t;
	//sf::Vector2f perSec, endF;
	float angleInDegrees;
	LivingEntity *target;
	ID_t targetID;

	sf::Vector2f pos;

	int projectileDamage;

	bool alive = true;
};


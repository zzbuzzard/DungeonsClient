#pragma once
#include "Entity.h"
class DamageText : public Entity
{
public:
	DamageText(pf pos_, int dmg, bool isXP = false);
	DamageText(Entity *follow, pf offset, int dmg, bool isXP = false);
	void update(GameState *state);
	void draw(sf::RenderWindow *window, GameState *state);
	bool isAlive();
private:
	void init();
	float life;
	bool isXP;
	sf::Vector2f wpos;
	Entity *follow = nullptr;

	sf::Text text;
};
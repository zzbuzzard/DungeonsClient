#pragma once
#include "SFML/Graphics.hpp"
#include "Item.h"

class GameState;

// Item Name
//  Stat increases

// If weapon,
//  Damage
//  Fire rate

class ItemHoverUI
{
public:
	ItemHoverUI();
	void update(GameState *state);
	void draw(GameState *state, sf::RenderWindow *window);
	void display(ITEM item);
private:
	ITEM currentlyDisplaying = I_NONE;
	bool isVisible;
	void setPos(sf::Vector2f pos);

	sf::RectangleShape panel;
	sf::Text titleText;
	sf::Text statsText;
};


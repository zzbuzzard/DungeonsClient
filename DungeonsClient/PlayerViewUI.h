#pragma once
#include "SFML/Graphics.hpp"
#include "Item.h"

// Bottom right of the screen.
// Display:
//  ___                   X
// |   | Name, level 26
// |___| O O O O O (items)
//   
//   Send friend request
//     Request trade

class Player;
class GameState;

class PlayerViewUI
{
public:
	PlayerViewUI();
	void setDisplay(Player *p);
	void close();
	void setPos(pf pos);
	void draw(GameState *state, sf::RenderWindow *window);
private:
	bool isVisible = false;

	sf::RectangleShape panel;
	sf::RectangleShape playerBox;
	sf::RectangleShape player;
	sf::RectangleShape gearBoxes[NUM_EQUIP_LOCS];
	sf::RectangleShape gearImages[NUM_EQUIP_LOCS];
	sf::Text nameText;

	const static float textHeight, playerImgSize, border, itemButtonBorder;
	const static int textFontSize;
};


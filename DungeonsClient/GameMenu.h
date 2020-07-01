#pragma once
#include "Button.h"
#include "Inventory.h"
#include "ChatManager.h"
#include "ItemHoverUI.h"
#include "XPBarUI.h"
#include "StatsText.h"

class GameState;

// We store equips
// But also 3 savedEquips[5]
#define LOADOUTS 3

#define INV_ALONG 4
#define INV_UP 3
// must * to make INV_SIZE in inventory.h

// The main panel is a WIDTHx500 rectangle.
// The lower section occupy a fixed number of game units

class GameMenu
{
public:
	GameMenu();
	void update(GameState *state);
	void draw(GameState *state, sf::RenderWindow *window);

private:
public:
	ChatManager chat;
	StatsText statsUI;
	bool isVisible = true;

private:
	void setLoadout(GameState *state, int n);
	int getLoadoutKeyboardPress();
	int currentLoadout = 0;

	ItemHoverUI hoverUI;
	XPBarUI xpBar;

	bool isDragging = false;
	int draggingInd;
	bool equipDragged;
	sf::RectangleShape dragBox;

	sf::Vector2f offset;

	Stats cachedStats;

	sf::RectangleShape panel;
	Button inventoryButtons[INV_SIZE]; sf::Vector2f invButtonPos[INV_SIZE];
	Button equipmentButtons[NUM_EQUIP_LOCS]; sf::Vector2f eqButtonPos[NUM_EQUIP_LOCS];
	//sf::Text statsText;

	Button loadoutButtons[LOADOUTS]; pf loadoutButtonPos[LOADOUTS];
	ITEM loadouts[LOADOUTS][NUM_EQUIP_LOCS];

	float sx;
};


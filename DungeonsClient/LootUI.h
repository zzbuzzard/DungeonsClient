#pragma once
#include "Button.h"
#include "PlayerInfo.h"
#include "SFML/Graphics.hpp"
#include <vector>

class GameState;

class LootUI
{
public:
	LootUI(PlayerInfo *pInfo_);

	void draw(sf::RenderWindow *window);
	void update(GameState *state);
private:
	void setLootUI(pi pos);

public:
	pi currentPos;

private:
	bool visible = false;
	PlayerInfo *pInfo;

	sf::RectangleShape bg;
	std::vector<Button> itemButtons;
};

// Only one instance.

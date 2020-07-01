#pragma once
#include "Types.h"

class PlayerInfo;

class XPBarUI
{
public:
	XPBarUI();
	void setPos(pf topLeft, float w);
	void draw(sf::RenderWindow *window);

	void update(PlayerInfo *pInfo);
private:
	exp_t currentXP = -1;

	sf::RectangleShape xpBar, xpBox;
	sf::Text levelText, xpProgressText;
};

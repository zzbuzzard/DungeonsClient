#include "pch.h"
#include "XPBarUI.h"
#include "PlayerInfo.h"
#include "Media.h"

static const float xpBarWidth = 375.0f, xpBarHeight = 40.0f;
static const sf::Color xpBarCol(0, 255, 0), xpBoxCol(0, 0, 0);
static const int levelCharSize = 60, progCharSize = 40;

XPBarUI::XPBarUI() {
	xpBar.setFillColor(xpBarCol);
	xpBar.setSize(pf(xpBarWidth, xpBarHeight));

	xpBox.setFillColor(xpBoxCol);
	xpBox.setSize(pf(xpBarWidth, xpBarHeight));

	levelText.setFont(g_pixelFont);
	levelText.setCharacterSize(levelCharSize);
	levelText.setFillColor(sf::Color::Black);

	xpProgressText.setFont(g_pixelFont);
	xpProgressText.setCharacterSize(progCharSize);
	xpProgressText.setFillColor(sf::Color::Black);
}

void XPBarUI::setPos(pf topLeft, float w) {
	float x = (w - xpBarWidth) / 2.0f + topLeft.x;
	float mid = w / 2 + topLeft.x;
	float y = topLeft.y;

	levelText.setPosition(mid, y+20.0f);
	xpBox.setPosition(x, y+50.0f);
	xpBar.setPosition(x, y+50.0f);
	xpProgressText.setPosition(mid, y+110.0f);
}

void XPBarUI::draw(sf::RenderWindow *window) {
	window->draw(xpBox);
	window->draw(xpBar);
	window->draw(levelText);
	window->draw(xpProgressText);
}

void XPBarUI::update(PlayerInfo *pInfo) {
	exp_t totXP = pInfo->getXP();
	if (totXP == currentXP) return;
	currentXP = totXP;

	level_t lev = pInfo->getPlayerLevel();
	exp_t remain = pInfo->getLeftoverXP();
	exp_t tot = PlayerInfo::getLevelUpXP(lev);

	float mul = remain / (float)tot;

	xpBar.setSize(pf(xpBarWidth * mul, xpBarHeight));

	std::cout << "updating" << endl;

	levelText.setString("Level " + std::to_string(lev));
	xpProgressText.setString(std::to_string(remain) + " / " + std::to_string(tot));

	util::centerSfText(levelText);
	util::centerSfText(xpProgressText);
}

#include "pch.h"
#include "StatsText.h"
#include "Media.h"
#include "GameState.h"

static const float gapSize = 50, height = 50, middleGap = 20, buttonSize = 90;
static const int fontSize = 40;

static float width;
static pf offset;
static pf textOffset(0, 0);

StatsText::StatsText(float w) {
	width = (w - gapSize) / 2.0f;

	float b = (height - buttonSize) / 2;
	offset = pf(width - b - buttonSize, b);

	//testPanel.setSize(pf(w, height*4+middleGap));
	//testPanel.setFillColor(sf::Color(150, 150, 150));

	pf pos(0, 0);

	for (int i = 0; i < NUM_STATS; i++) {
		statTexts[i].setFont(g_pixelFont);
		statTexts[i].setFillColor(sf::Color::Black);
		statTexts[i].setCharacterSize(fontSize);
		
		auto rect = statTexts[i].getGlobalBounds();
		statTexts[i].setOrigin(rect.left, rect.top);

		statUpBoxes[i].setSize(pf(buttonSize, buttonSize));
		statUpBoxes[i].setTexture(&textures[T_PLUS_UNPRESSED]);

		TLs[i] = pos;

		if (i != 3) {
			pos.y += height;
			if (i % 4 == 1)
				pos.y += middleGap;
		}
		else {
			pos = pf(width + gapSize, 0);
		}
	}
}

void StatsText::setPos(pf pos) {
	//testPanel.setPosition(pos);
	for (int i = 0; i < NUM_STATS; i++) {
		statTexts[i].setPosition(TLs[i] + textOffset + pos);
		statUpBoxes[i].setPosition(TLs[i] + offset + pos);
	}
}

void StatsText::draw(GameState *state, sf::RenderWindow *window) {
	update(state, &state->pInfo);

	//window->draw(testPanel);
	for (int i = 0; i < NUM_STATS; i++) {
		window->draw(statTexts[i]);
	}

	if (state->pInfo.getStatPoints() > 0) {
		for (int i = 0; i < NUM_STATS; i++) {
			if (state->pInfo.canUpgrade((Stat)i)) {
				if (hoveredInd == i) {
					statUpBoxes[i].setTexture(&textures[T_PLUS_PRESSED]);
				}
				else {
					statUpBoxes[i].setTexture(&textures[T_PLUS_UNPRESSED]);
				}
				window->draw(statUpBoxes[i]);
			}
		}
	}
}

void StatsText::updateDisplay(const Stats &stats) {
	for (int i = 0; i < NUM_STATS; i++) {
		stat_t t = stats.stats[i];
		std::string x = statNames[i];
		if (x.size() == 2) x += " ";
		statTexts[i].setString(x + " " + std::to_string(t));
	}
}

void StatsText::update(GameState *state, PlayerInfo *pInfo) {
	if (pInfo->getStatPoints() > 0) {
		if (mouseReleased && hoveredInd != -1 && pInfo->canUpgrade((Stat)hoveredInd)) {
			pInfo->useStatPoint((Stat)hoveredInd);
		}
		bool w = false;
		if (mouseDown) {
			pf m = state->getMousePosWorld();
			for (int i = 0; i < NUM_STATS; i++) {
				pf size = statUpBoxes[i].getSize();
				pf pos = statUpBoxes[i].getPosition() + size * 0.5f;
				size *= 0.6f;

				if (m.x > pos.x - size.x / 2 && m.x < pos.x + size.x / 2) {
					if (m.y > pos.y - size.y / 2 && m.y < pos.y + size.y / 2) {
						hoveredInd = i;
						w = true;
					}
				}
			}
		}
		if (!w) hoveredInd = -1;
	}
	else {
		hoveredInd = -1;
	}
}

// 1) StatPoints should be stored in pInfo
// 2) Client + server's gainXP functions should both check for a level up,
//    then add the stat points on to statPoints.
// 3) P_SPEND_STAT_POINT n
// 4) For custom received BaseStats, calculated statPoints

// We hit level 5.
//  PlayerInfo checks number for this level and adds to statPoints.
//  Server also adds to statPoints.

// The buttons appear.
// We click a button and the change is immediate
//  (but we increment inventoryCountThing + request a baseStats thing and we check that it is equal)
// 

// What if we don't spend the stat point and DC?
// Well, when we reconnect, we can work out stat points from our level and the stats.
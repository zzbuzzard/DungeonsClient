#pragma once
#include "Types.h"
#include "Stats.h"
#include "Util.h"

class GameState;
class PlayerInfo;

class StatsText
{
public:
	// Width is allowed width
	StatsText(float w);

	void setPos(pf pos);
	void draw(GameState *state, sf::RenderWindow *window);
	void updateDisplay(const Stats &stats);
private:
	void update(GameState *state, PlayerInfo *pInfo);
public:
private:
	int hoveredInd = -1;
	pf TLs[NUM_STATS];
	sf::Text statTexts[NUM_STATS];
	sf::RectangleShape statUpBoxes[NUM_STATS];

	//sf::RectangleShape testPanel; //todo: remove
};


// HP  180     INT 53
// MP  105     REC 60
// DEF 80      WIS 25
// SPD 110     ATK 100

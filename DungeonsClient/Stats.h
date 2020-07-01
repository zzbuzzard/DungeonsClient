#pragma once
#include <string>
#include "Types.h"

#define NUM_STATS 8
enum class Stat {
	HP, MP, DEF, SPD, ATK, WIS, REC, INT
};

extern std::string statNames[NUM_STATS];
class Stats;
extern Stats playerStatsBase;

class Stats
{
public:
//  hp         HP                           : One upgrade gives you +5 HP
//	mp         MP for special               : One upgrade gives you +5 MP
//	def        Subtracted from each hit     : One upgrade gives you +1 DEF   (Cap is 1 at level 10, 2 at level 20, etc.)
//	spd        Movement speed               : One upgrade gives you +1 SPD
//	atk        Attack damage                : One upgrade gives you +1 ATK
//	wis        Special multiplier           : One upgrade gives you +1 WIS
//	rec        HP recovery                  : One upgrade gives you +1 REC   (change to +2 ?)
//	int        MP recovery                  : One upgrade gives you +1 INT   (change to +2 ?)

	stat_t stats[NUM_STATS];

	Stats();
	Stats(stat_t x[NUM_STATS]);
	Stats(stat_t hp_, stat_t mp_, stat_t def_, stat_t spd_, stat_t atk_, stat_t wis_, stat_t rec_, stat_t inte_);
	Stats operator+(const Stats &ot);
	void operator+=(const Stats &ot);
	bool operator==(const Stats &ot);
	bool operator!=(const Stats &ot);

	life_t getHP();
	float getRecTime();
	float getAtkMultiplier();

	mp_t getMP();
	float getMpRecTime();
	float getWisMultiplier();
	static float getWisMultiplierOf(stat_t wis);

	float getSpeed();
	def_t getDef();

	int getSpentStatPointsOverall();
	int getSpareStatPointsOverall(level_t lev);

	int getSpentStatPoints(Stat stat);
	static int getStatPointsAt(level_t lev);
};

// Stats are never sent by the server.
//  {Apart from when we load up, get base stats}

// STATS :
//  Players have base stats.
//   (can be increased)
//  All equipment items have stat boosts

// So a player's stats = base + equipment
// 

// When we connect to the server
//  The server gets our base stats and inventory and equipment from the webserver - or just placeholder for now
//  In our hello, we send the player
//    - Their ID
//    - Their spawn position
//    - The overworld's seed
//    - Their inventory {and equipment}
//    - Their base stats

// We then both process that to produce their stats.

// When the player upgrades a stat, they change their stat object and send a thing to the server saying we ate it.
//  P_USE_INVENTORY(3)
// The server replies to this with another P_PLAYER_STATUS = inventory + base stats

// 
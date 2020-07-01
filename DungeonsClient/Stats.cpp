#include "pch.h"
#include "Stats.h"
#include "Util.h"
#include <algorithm> // for std::max

std::string statNames[NUM_STATS] = {
	"HP", "MP", "DEF", "SPD", "ATK", "WIS", "REC", "INT"
};

Stats playerStatsBase = Stats(150, 100, 0, 10, 10, 10, 10, 10);

Stats::Stats() {
	for (int i = 0; i < NUM_STATS; i++)
		stats[i] = 0;
}

Stats::Stats(stat_t x[NUM_STATS]) {
	for (int i = 0; i < NUM_STATS; i++)
		stats[i] = x[i];
}

Stats::Stats(stat_t hp, stat_t mp, stat_t def, stat_t spd, stat_t atk, stat_t wis, stat_t rec, stat_t int_) {
	stats[(int)Stat::HP] = hp;
	stats[(int)Stat::MP] = mp;
	stats[(int)Stat::DEF] = def;
	stats[(int)Stat::SPD] = spd;
	stats[(int)Stat::ATK] = atk;
	stats[(int)Stat::WIS] = wis;
	stats[(int)Stat::REC] = rec;
	stats[(int)Stat::INT] = int_;
}


Stats Stats::operator+(const Stats &ot) {
	stat_t y[NUM_STATS];
	for (int i = 0; i < NUM_STATS; i++)
		y[i] = stats[i] + ot.stats[i];
	return Stats(y);
}


void Stats::operator+=(const Stats &ot) {
	for (int i = 0; i < NUM_STATS; i++)
		stats[i] += ot.stats[i];
}


bool Stats::operator==(const Stats &ot) {
	for (int i = 0; i < NUM_STATS; i++)
		if (stats[i] != ot.stats[i])
			return false;
	return true;
}
bool Stats::operator!=(const Stats &ot) {
	return !(*this == ot);
}

float Stats::getSpeed() {
	return 5.0f + 0.08f * stats[(int)Stat::SPD];
}

life_t Stats::getHP() {
	return stats[(int)Stat::HP];
}

mp_t Stats::getMP() {
	return stats[(int)Stat::MP];
}

// ATK = 100 means you do 2.5x weapon's damage
// ATK = 30 means you do the weapon's damage

// a + 100x = 2.7
// a + 30x = 1
// 70x = 2
// x = 2/70

float Stats::getAtkMultiplier() {
	static float x = 1.7f / 70.0f;
	static float a = 1 - 30 * x;

	return a + x * stats[(int)Stat::ATK];
}

// Copied from atkmultiplier
float Stats::getWisMultiplier() {
	return getWisMultiplierOf(stats[(int)Stat::WIS]);
}

float Stats::getWisMultiplierOf(stat_t wis) {
	static float x = 1.5f / 70.0f;
	static float a = 1 - 30 * x;

	return a + x * wis;
}


// OLD FORMULA 6/(6+rec) but it was boringly slow
// NEW FORMULA 5.0/(5+rec) more rec

// NTS: 4 is okaaay
//      9 is OVERPOWERED (with 5 formula, thats rec = 40)

float Stats::getRecTime() {
	return 5.0f / (8.0f + std::max((stat_t)0, stats[(int)Stat::REC]));
}

// Copied from HP
float Stats::getMpRecTime() {
	return 5.0f / (5.0f + std::max((stat_t)0, stats[(int)Stat::INT]));
}


def_t Stats::getDef() {
	return stats[(int)Stat::DEF];
}

int Stats::getSpentStatPointsOverall() {
	int t = 0;
	for (int i = 0; i < 8; i++) {
		t += getSpentStatPoints((Stat)i);
	}
	return t;
}

int Stats::getSpentStatPoints(Stat stat) {
	int c = stats[(int)stat] - playerStatsBase.stats[(int)stat];
	if ((int)stat <= 1) {
		if (c % 5 != 0) {
			cout << "E R R O R : Inputted baseStats cannot be made only by stat point upgrades" << endl;
			return 200;
		}
		return c / 5;
	}
	return c;
}

int Stats::getSpareStatPointsOverall(level_t lev) {
	int t = 0;
	for (level_t i = 2; i <= lev; i++)
		t += getStatPointsAt(i);

	//cout << "Total earned stat points = " << t << endl;
	int sp = getSpentStatPointsOverall();
	//cout << "Total spent stat points = " << sp << endl;

	return t - sp;
}


int Stats::getStatPointsAt(level_t lev) {
	if (lev <= 1) return 0;
	if (lev <= 39) return 1;
	if (lev == 50) return 0;
	if (lev <= 69) return 2;
	if (lev <= 89) return 3;
	if (lev <= 100) return 4;
	return 0;
}


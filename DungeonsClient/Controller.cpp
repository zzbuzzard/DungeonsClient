#include "pch.h"
#include "Controller.h"
#include "State.h"

#include <fstream>

State *g_state, *g_savedState;
bool g_gameState = false, windowFocus = true;
ConnectionManager *connection = nullptr;
//int Coins;
//void loadCoins() {
//	std::ifstream fin("coins");
//	if (fin.good()) {
//		fin >> Coins;
//	}
//	else Coins = 10;
//}
//void saveCoins() {
//	std::ofstream fout("coins");
//	fout << Coins;
//}
//int getCoins() { return Coins; }
//void addCoins(int a) {
//	Coins += a;
//	saveCoins();
//}

void moveStateToSave() {
	g_savedState = g_state;
	g_state = nullptr;
}

void loadSavedState() {
	delete g_state;
	g_state = g_savedState;
	g_savedState = nullptr;
}

void deleteState() {
	delete g_state;
}

Controller::Controller() {
}

Controller::~Controller() {
}

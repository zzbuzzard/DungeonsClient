#pragma once

class State;
class ConnectionManager;

extern State *g_state, *g_savedState;
extern bool g_gameState, windowFocus;
extern ConnectionManager *connection;

void deleteState();

void moveStateToSave();
void loadSavedState();

//void loadCoins();
//int getCoins();
//void addCoins(int a);

class Controller
{
public:
	Controller();
	~Controller();
};


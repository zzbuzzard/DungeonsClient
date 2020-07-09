#pragma once
#include <SFML/Network.hpp>
#include "Util.h"

class GameState;

class ConnectionManager
{
public:
	ID_t getMyID();
	void connect();
	void checkForUpdates();
	bool isConnected();
	void send(sf::Packet packet);
	// void login();
	// bool authenticated = false;

	void handleToken(std::string token);
	bool tokenReady();

private:
	std::string token = "";

	sf::TcpSocket socket;
	pi myPos;
	bool connected = false;
	ID_t myID = -1;
};


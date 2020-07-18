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

	// Handles a response string from the token script: either a failure or ID [space] token
	void handleToken(std::string token);
	bool tokenReady();

	void setUsername(const std::string &username);
	const std::string &getUsername();

private:
	std::string token = "";

	std::string myUsername;

	sf::TcpSocket socket;
	pi myPos;
	bool connected = false;
	ID_t myID = -1;
};


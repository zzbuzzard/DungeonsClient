#include "pch.h"
#include "ConnectionManager.h"
#include "Packet.h"
#include "Controller.h"
#include "GameState.h"

#include <iostream>

using std::cout;
using std::endl;

#define PORT 38383
#define IP "81.106.47.202"

void ConnectionManager::connect() {
	if (!tokenReady()) {
		cout << "Trying to connect but the token aint ready..." << endl;
		return;
	}

	cout << "Connecting to server..." << endl;

	sf::Socket::Status status = socket.connect(IP, PORT);

	if (status == sf::Socket::Done) {
		cout << "Connected to server" << endl;
		connected = true;
	}
	else{
		connected = false;
		cout << "Failed to connect: ";
		if (status == sf::Socket::Error) {
			cout << "Error" << endl;
		}
		if (status == sf::Socket::Disconnected) {
			cout << "Disconnected" << endl;
		}
		if (status == sf::Socket::NotReady) {
			cout << "Not ready" << endl;
		}
	}

	if (connected) {
		sf::Packet clientHello;
		clientHello.append(&P_CLIENT_HELLO, sizeof(Packet));

		clientHello.append(&myID, sizeof(ID_t));

		int16_t num = token.size();
		clientHello.append(&num, sizeof(int16_t));
		for (char c : token)
			clientHello.append(&c, sizeof(char));

		cout << "Sending client hello" << endl;
		socket.send(clientHello);

		socket.setBlocking(false);
	}
}

void ConnectionManager::checkForUpdates() {
	while (true) {
		sf::Packet packet;
		sf::Socket::Status status = socket.receive(packet);
		if (status == sf::Socket::Done) {
			serverLastHeard = totalTime;

			// Data has been received
			const void *data = packet.getData();
			Packet packet_t = getPacketType(data);

			// Handle the different types of packet
			switch (packet_t) {
			case P_SERVER_HELLO:
				if (g_gameState)
					((GameState*)g_state)->handleHello(data);
				break;
			case P_UPDATE:
				//cout << "Lovely to hear an update." << endl;
				if (g_gameState)
					((GameState*)g_state)->handleUpdate(data);
				break;
			case P_PLAYER_EQUIPS:
				//cout << "Lovely to hear an update." << endl;
				if (g_gameState)
					((GameState*)g_state)->handleEquipmentUpdate(data);
				break;
			case P_BASE_INV:
				//cout << "Got a base stats + inv update" << endl;
				if (g_gameState) 
					((GameState*)g_state)->handleBaseStatsInv(data);
				break;
			case P_LOOT:
				if (g_gameState) 
					((GameState*)g_state)->handleLoot(data);
				break;
			case P_DUNGEON_ENTERED:
				if (g_gameState)
					((GameState*)g_state)->handleEnterDungeon(data);
				break;
			case P_YOU_DIED:
				if (g_gameState)
					((GameState*)g_state)->handlePlayerDeath(data);
				break;
			case P_MSG:
				if (g_gameState)
					((GameState*)g_state)->handleMessage(data);
				break;
			case P_USE_SPECIAL:
				if (g_gameState)
					((GameState*)g_state)->handleSpecial(data);
				break;
			case P_CHANGE_TILE:
				if (g_gameState)
					((GameState*)g_state)->handleChangeTile(data);
				break;
			case P_TL_TILE:
				if (g_gameState)
					((GameState*)g_state)->handleTlTile(data);
				break;
			case P_TL_TILES:
				if (g_gameState)
					((GameState*)g_state)->handleTlTiles(data);
				break;
			case P_CONFIRM_ITEM:
				if (g_gameState)
					((GameState*)g_state)->handleConfirmedItem(data);
				break;
			case P_PLAYER_USERNAME:
				if (g_gameState)
					((GameState*)g_state)->handleRequestedUsername(data);
				break;
			}
		}
		else {
			if (status == sf::Socket::Disconnected) {
				cout << "Disconnected" << endl;
				connected = false;
			}
			break;
		}
	}
}

bool ConnectionManager::isConnected() {
	return connected;
}


void ConnectionManager::send(sf::Packet packet) {
	socket.send(packet);
}

ID_t ConnectionManager::getMyID() {
	return myID;
}

void ConnectionManager::handleToken(std::string token_) {
	cout << "Handling token " << token_ << endl;

	std::string idNum = "";
	int spaceInd = -1;
	for (int i = 0; i < token_.size(); i++) {
		if (token_[i] == ' ') {
			spaceInd = i;
			break;
		}
		if (token_[i] < '0' || token_[i] > '9') break; // invalid
		idNum.push_back(token_[i]);
	}

	if (spaceInd == -1) {
		cout << "Token was invalid" << endl;
		return;
	}

	myID = std::stoi(idNum);
	cout << "My id = " << myID << endl;

	token = token_.substr(spaceInd+1);
}

bool ConnectionManager::tokenReady() {
	return token != "";
}


void ConnectionManager::setUsername(const std::string &username) {
	myUsername = username;
}

const std::string &ConnectionManager::getUsername() {
	return myUsername;
}

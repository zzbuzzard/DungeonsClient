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

	socket.setBlocking(false);
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
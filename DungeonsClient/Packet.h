#pragma once
#include <cstdint>
#include "Types.h"

const Packet
	P_SERVER_HELLO = 0,  // Hello, ID, spawnX, spawnY, overworld seed
	P_LOGIN_REQUEST = 1, // Username, Password [totally unencrypted YIKES] : could instead send username, time, hash of password with time : OR TOKEN SYSTEM
	P_LOGIN_RESULT = 2,  // Y/N
	P_JOIN_GAME = 3,     // GameState ID
	P_SEND_MSG = 4,      // Message
	P_MSG = 5,           // Speaker ID, Message
	P_MOVE = 6,          // Movement direction, {Movement number}
	P_UPDATE = 7,        // *Entire* gamestate, all enemies and players
	P_TARGET = 8,        // Targetted ID
	P_BASE_INV = 9,      // Base stats, inventory
	P_INV_MOVE = 10,     // Requests to manipulate inventory. Followed by one or two int8_ts
	P_INV_EQUIP = 11,
	P_INV_UNEQUIP = 12,
	P_LOOT = 13,         // X, Y, NumberITEMs [int8_t], ITEMs
	P_PICKUP_LOOT = 14,  // Index [int8_t] (in the chest's list we are currently standing on)
	P_ENTER_DUNGEON = 15,// Just that
	P_DUNGEON_ENTERED = 16, // BiomeType, seed
	P_YOU_DIED = 17,     // Self explanatory
	P_USE_SPECIAL = 18,  // userID, usedOnID [server ensures userID is correct, and that usedOnID is valid (eg player->player for heals only)]
	P_PLAYER_EQUIPS = 19,// Number [PlayerID ITEM*5]
	P_RESPAWN = 20,      // Sent after dying
	P_USE_STAT = 21,     // Stat to be spent on
	P_CHANGE_TILE = 22,  // x, y, newtile
	P_TL_TILE = 23,      // x, y, TopLayerTileType
	P_TL_TILES = 24,     // type, num, xys
	P_DROP_ITEM = 25,    // index in inventory. adds to loot pool. replies with P_LOOT
	P_CONFIRM_ITEM = 26; // x, y, item_ind (confirm it was picked up)

// Each GameState has its own update packet, which it sends to all its participants.
// When a movement thing is received, it is forwarded to the appropriate GameState
// Using a map I suppose

// So, the server's flow is this
//  Check for new connections
//  Check for input. If so, forward it to the appropriate GameState to handle.
//  Update all GameStates
//  Send updates from all GameStates

// And the client's flow is this
//  GameState updates game
//  GameState check for player input and send off using connection

Packet getPacketType(const void *data);

#pragma once
#include <cstdint>
#include "SFML/Graphics.hpp"

typedef uint16_t player_count_t;
typedef uint16_t enemy_count_t;

typedef uint8_t target_count_t;
typedef uint8_t Packet;

typedef int16_t ID_t;      // max number of entities 32,000
typedef int seed_t;
typedef int16_t damage_t;  // max damage 32,000
typedef int16_t def_t;     // max defence 32,000
typedef int32_t life_t;    // max life 2 billion
typedef uint8_t range_t;   // max range 256

typedef int16_t mp_t; // max MP 32,000
typedef int16_t stat_t; // stat points go up to a max of 32,000. 256 is just a little too low.

typedef int8_t proc_moves_t;
typedef int8_t item_num_t; // max drops from an enemy 127 (signed just safer)
typedef uint8_t msg_len_t; // max message length 255 chars

typedef int16_t pos_t;    // max map size 32,000 x 32,000

typedef sf::Vector2<pos_t> pi;
typedef sf::Vector2f pf;

typedef int32_t exp_t; // At level 100, your total XP = just over 10,000,000 = 10mil << 2bil
typedef int8_t level_t; // Level is between 1 and 100

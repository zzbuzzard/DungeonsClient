#pragma once
#include <SFML/Graphics.hpp>
#include "CSMode.h"

// Can change the order of these
// (along as the path array order is changed too)
#define NUM_TEXTURES 92
enum TEXTURE {
	T_PLAYER,

	T_ENEMY1,
	T_ENEMY2,
	T_ICE_ENEMY,
	T_MUNCHER,
	T_DARK_MUNCHER,
	T_EYE_SPORE,
	T_MUMMY_HEAD,
	T_SCORPION,
	T_FIRE_MAGE,
	T_ARCHER,
	T_SLIME,
	T_ELF_ARCHER,
	T_ELF_KNIGHT,
	T_CURSED_ELF_KNIGHT,
	T_LEAFLING,
	T_SPIDER,

	T_SAND_DEMON,    //2x2
	T_CURSED_CACTUS, //3x3
	T_SAND_GOLEM,    //2x2
	T_MUMMY,         //2x2
	T_ENT,			 //2x2
	T_MEGA_MUNCHER,  //2x2

	T_ARROW,
	T_GOLD_ARROW,
	T_FIREBALL,
	T_SPINE,
	T_GOO_BALL,
	T_SAND_SPIKE,
	T_DUST,
	T_HEAL_DUST,
	T_THORN,
	T_SHARD,
	T_LEAF,

	T_TILE_SAND,
	T_TILE_WALL,
	T_TILE_GRASS,
	T_TILE_SNOW,
	T_TILE_VOLCANO,
	T_TILE_DARK_SAND,	
	T_TILE_WALL_WOOD,
	T_TILE_GRASS_BLOOD,
	T_TILE_GRASS_DARK,
	T_TILE_GRASS2,
	T_TILE_MUD,

	T_TL_CACTUS,
	T_TL_ROCK,
	T_TL_SPIKE_FLOOR,
	T_TL_LAVA_POOL,
	T_TL_SPIKY_TREE,

	T_DUNGEON_ENTRANCE_GRASS,
	T_DUNGEON_ENTRANCE_DESERT,

	T_TEST_HELMET,
	T_CURSED_RAGS,
	T_RUSTY_HELMET,
	T_SLIME_FACEMASK,
	T_CURSED_ELVEN_HELMET,
	T_VINE_HELMET,

	T_TEST_ARMOUR,
	T_PLANT_ARMOUR,
	T_RUSTY_ARMOUR,
	T_SAND_CAPE,
	T_SCORPION_SCALE_ARMOUR,
	T_WARM_CLOAK,
	T_COBWEB_ARMOUR,

	T_TEST_ACCESSORY,
	T_RECOVERY_CHARM,
	T_DESERT_RING,
	T_CURSED_CACTUS_EYE,
	T_SANDSTORM_AMULET,
	T_SLIME_ESSENCE,
	T_CORRUPT_AMULET,

	T_TEST_BOW,
	T_GOLDEN_BOW,
	T_DESERT_BOW,
	T_EARTH_STAFF,
	T_FIRE_STAFF,
	T_SAND_SWORD,
	T_CACTUS_SWORD,
	T_SCORPION_DAGGER,
	T_ELVEN_BLADE,
	T_ELVEN_BOW,
	T_SPIDER_FANG_DAGGER,

	T_HOLY_ORB,

	T_CHEST_1,
	T_CHEST_2,
	T_CHEST_3,
	T_CHEST_4,

	T_CIRCLE,
	T_BLACK,
	T_PLUS_UNPRESSED,
	T_PLUS_PRESSED,

	T_NONE = -1
};

#ifdef CLIENT
extern sf::Font g_pixelFont;
extern sf::Texture textures[NUM_TEXTURES];
extern std::string texturePaths[NUM_TEXTURES];

class Media
{
public:
	static void PreLoad();
	static void LoadNextTexture();
	static void FinishUp();
private:
	//static void LoadTextures();
	static void LoadFonts();
};

#endif
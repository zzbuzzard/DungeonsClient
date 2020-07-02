#include "pch.h"
#include "Media.h"
#include <iostream>

// 1) Enum and reference all textures by index in array
//    Advantage: they're all close together in memory, prevents pointer hopping when drawing the scene

// Please note that I didn't compare speeds, and that this is an entirely baseless claim


#ifdef CLIENT
// All have Textures/ at the start
std::string texturePaths[NUM_TEXTURES] = {
	"Textures/Skins/player.png",

	"Textures/Enemies/enemy1.png",
	"Textures/Enemies/enemy2.png",
	"Textures/Enemies/ice_enemy.png",
	"Textures/Enemies/muncher.png",
	"Textures/Enemies/dark_muncher.png",
	"Textures/Enemies/eye_spore.png",
	"Textures/Enemies/mummy_head.png",
	"Textures/Enemies/scorpion.png",
	"Textures/Enemies/mage.png",
	"Textures/Enemies/sand_archer.png",
	"Textures/Enemies/slime.png",
	"Textures/Enemies/elf_archer.png",
	"Textures/Enemies/elf_knight.png",
	"Textures/Enemies/cursed_elf_knight.png",
	"Textures/Enemies/leafling.png",
	"Textures/Enemies/spider.png",

	"Textures/Enemies/sand_demon.png",
	"Textures/Enemies/cursed_cactus.png",
	"Textures/Enemies/sand_golem.png",
	"Textures/Enemies/mummy.png",
	"Textures/Enemies/ent.png",
	"Textures/Enemies/mega_muncher.png",

	"Textures/Projectiles/arrow.png",
	"Textures/Projectiles/gold_arrow.png",
	"Textures/Projectiles/fireball.png",
	"Textures/Projectiles/spine.png",
	"Textures/Projectiles/goo_ball.png",
	"Textures/Projectiles/sand_spike.png",
	"Textures/Projectiles/dust.png",
	"Textures/Projectiles/heal_dust.png",
	"Textures/Projectiles/thorn.png",
	"Textures/Projectiles/shard.png",
	"Textures/Projectiles/leaf.png",

	"Textures/Tiles/sand.png",
	"Textures/Tiles/wall.png",
	"Textures/Tiles/grass.png",
	"Textures/Tiles/snow.png",
	"Textures/Tiles/volcano.png",
	"Textures/Tiles/dark_sand.png",
	"Textures/Tiles/wall_wood.png",
	"Textures/Tiles/grass_blood.png",
	"Textures/Tiles/grass_long.png",
	"Textures/Tiles/grass2.png",
	"Textures/Tiles/mud.png",

	"Textures/Tiles/cactus.png",
	"Textures/Tiles/rock.png",
	"Textures/Tiles/spike_floor.png",
	"Textures/Tiles/lava_pool.png",
	"Textures/Tiles/spiky_tree.png",

	"Textures/Tiles/grass_dungeon_entrance.png",
	"Textures/Tiles/desert_dungeon_entrance.png",

	"Textures/Items/test_helmet.png",
	"Textures/Items/cursed_rags.png",
	"Textures/Items/rusty_helmet.png",
	"Textures/Items/slime_facemask.png",
	"Textures/Items/cursed_elven_helmet.png",
	"Textures/Items/vine_helmet.png",
	"Textures/Items/sand_helmet.png",

	"Textures/Items/test_armour.png",
	"Textures/Items/plant_armour.png",
	"Textures/Items/rusty_armour.png",
	"Textures/Items/sand_cape.png",
	"Textures/Items/scorpion_scale_armour.png",
	"Textures/Items/warm_cloak.png",
	"Textures/Items/cobweb_armour.png",

	"Textures/Items/test_accessory.png",
	"Textures/Items/recovery_charm.png",
	"Textures/Items/desert_ring.png",
	"Textures/Items/cursed_cactus_eye.png",
	"Textures/Items/sandstorm_amulet.png",
	"Textures/Items/slime_essence.png",
	"Textures/Items/corrupt_amulet.png",

	"Textures/Items/test_bow.png",
	"Textures/Items/golden_bow.png",
	"Textures/Items/desert_bow.png",
	"Textures/Items/earth_staff.png",
	"Textures/Items/fire_staff.png",
	"Textures/Items/sand_sword.png",
	"Textures/Items/cactus_sword.png",
	"Textures/Items/scorpion_dagger.png",
	"Textures/Items/elven_blade.png",
	"Textures/Items/elven_bow.png",
	"Textures/Items/spider_fang_dagger.png",

	"Textures/Items/holy_orb.png",

	"Textures/Chests/chest_1.png",
	"Textures/Chests/chest_2.png",
	"Textures/Chests/chest_3.png",
	"Textures/Chests/chest_4.png",

	"Textures/Misc/circle.png",
	"Textures/Tiles/black.png",
	"Textures/Misc/plus_unpressed.png",
	"Textures/Misc/plus_pressed.png",
};


sf::Font g_pixelFont;
sf::Texture textures[NUM_TEXTURES];

void Media::PreLoad() {
	for (int i = 0; i < NUM_TEXTURES; i++) textures[i] = sf::Texture();
	LoadFonts();
}

void Media::FinishUp() {
	//textures[T_BUTTON].setSmooth(true);
}


void Media::LoadNextTexture() {
	static int i = 0;
	textures[i].loadFromFile(texturePaths[i]);
	i++;
}

void Media::LoadFonts() {
	g_pixelFont = sf::Font();
	g_pixelFont.loadFromFile("Fonts/pixelFont.ttf");
	//const_cast<sf::Texture&>(g_pixelFont.getTexture(20)).setSmooth(false);
}
#endif
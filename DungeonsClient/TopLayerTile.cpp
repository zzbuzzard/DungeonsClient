#include "pch.h"
#include "TopLayerTile.h"
#include "State.h"

TLTileData *tileTypes[NUM_TL_TILES] = {
	new TLTileData(T_TL_ROCK, false),
	new TLTileData(T_TL_CACTUS, false),
	new TLTileData(T_TL_BUSH, false),

	new DamageTileData(T_TL_LAVA_POOL, 0.15f, 15),

	new DamageTileData(T_TL_SPIKE_FLOOR, 0.1f, 5, 5.5f),
	new DamageTileData(T_TL_SPIKY_TREE, 0.4f, 30, 1.0f),
};


TLTileData::TLTileData(TEXTURE texture_, bool walkable_, bool damaging_, float lifetime_)
	: texture(texture_), damaging(damaging_), walkable(walkable_), lifetime(lifetime_)
{
}
DamageTileData::DamageTileData(TEXTURE texture_, float damageCool_, damage_t damage_, float lifetime_)
	: TLTileData(texture_, true, true, lifetime_), damageCool(damageCool_), damage(damage_)
{
}

// Entity stuff

TopLayerTile::TopLayerTile(pi pos_, TLTileType tileType_)
	: Entity(E_NONE, (tileTypes[static_cast<int>(tileType_)]->damaging ? ET_TOP_LAYER_TILE_DMG : ET_TOP_LAYER_TILE), pos_),
	tileType(tileType_)
{
#ifdef CLIENT
	setBoxSize();
	setBoxTexture(getData()->texture);
#endif
	canWalk = getData()->walkable;
}
void TopLayerTile::update(GameState *state) {
}
bool TopLayerTile::isAlive() {
	return true;
}
TLTileData *TopLayerTile::getData() {
	return tileTypes[static_cast<int>(tileType)];
}


static const float fadeIn = 0.15f, fadeOut = 0.4f;

TimedTopLayerTile::TimedTopLayerTile(pi pos_, TLTileType tileType_)
	: TopLayerTile(pos_, tileType_), life(tileTypes[static_cast<int>(tileType_)]->lifetime), livedFor(0)
{
}
void TimedTopLayerTile::update(GameState *state) {
	life -= deltaTime;
	livedFor += deltaTime;

#ifdef CLIENT
	int a = 255;

	if (livedFor <= fadeIn) {
		a = livedFor / fadeIn * 255;
	}

	if (life <= fadeOut) {
		a = life/fadeOut * 255;
	}

	if (a < 0) a = 0;
	if (a > 255) a = 255;
	setBoxTint(sf::Color(255, 255, 255, a));
#endif
}
bool TimedTopLayerTile::isAlive() {
	return life > 0;
}

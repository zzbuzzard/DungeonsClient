#pragma once
#include "Types.h"
#include "Media.h"
#include "Entity.h"

// Top layer tile type : one instance stores the information to produce an entity
// E.g.  a rock would be texture=T_ROCK, walkable = false, damaging=false
//       lava would be walkable=true damaging=true and actually a DamageTileType
class TLTileData {
public:
	TLTileData(TEXTURE texture_, bool walkable_, bool damaging_ = false, float lifeTime_ = -1);

	const TEXTURE texture;
	const bool damaging;
	const bool walkable;
	const float lifetime;
};

class DamageTileData : public TLTileData {
public:
	DamageTileData(TEXTURE texture_, float damageCool_, damage_t damage_, float lifetime_ = -1);

	const float damageCool;
	const damage_t damage;
};

#define NUM_TL_TILES 5
enum class TLTileType : uint8_t {
	ROCK,
	CACTUS,
	SPIKE_FLOOR,
	LAVA_POOL,
	SPIKY_TREE,
};

extern TLTileData *tileTypes[NUM_TL_TILES];

class GameState;
class TopLayerTile : public Entity
{
public:
	TopLayerTile(pi pos_, TLTileType tileType_);
	void update(GameState *state); // do nothing
	bool isAlive(); // always true

	TLTileData *getData();

	const TLTileType tileType;
};

class TimedTopLayerTile : public TopLayerTile
{
public:
	TimedTopLayerTile(pi pos_, TLTileType tileType_);
	void update(GameState *state); // reduce life
	bool isAlive(); // life > 0

	float life;
	float livedFor;
};




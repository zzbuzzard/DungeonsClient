#pragma once
#include "CSMode.h"

#include <SFML/Graphics.hpp>
#include "Util.h"

#ifdef CLIENT
#include "Media.h"
#endif

enum EntityType {
	ET_PLAYER,
	ET_ENEMY,
	ET_PROJECTILE,
	ET_TOP_LAYER_TILE,
	ET_TOP_LAYER_TILE_DMG,
	ET_NO_BOX,
};

#define NUM_ENTITY_SPECIES 21
enum EntitySpecies : uint8_t {
	E_PLAYER,	
	E_ENEMY1, // Desert
	E_ENEMY2,
	E_FIRE_MAGE,

	E_SCORPION,
	E_SLIME,
	E_ARCHER,
	E_SAND_GOLEM,

	E_CURSED_CACTUS,
	E_MUMMY_BOSS, // Pyramid: TODO
	E_MUNCHER, // Grasslands
	E_EYE_SPORE,

	E_DARK_MUNCHER,
	E_ELF_ARCHER,
	E_ELF_KNIGHT,
	E_CURSED_ELF_KNIGHT,

	E_LEAFLING,
	E_ENT,
	E_SPIDER,
	E_MEGA_MUNCHER,

	E_ICE_ENEMY, // Tundra

	E_NONE = -1,
};

typedef std::pair<EntitySpecies, float> spawnPair;
typedef std::vector<spawnPair> spawnVector;

extern bool speciesCombatStatsConstant[NUM_ENTITY_SPECIES];

class GameState;
class Entity;

Entity* spawnEntityID(EntitySpecies spec, ID_t ID, const pi &pos);
EntitySpecies randomEntityFromVector(const spawnVector &vec, int a=0, int b=-1);

class Entity
{
public:
	Entity(EntitySpecies spec_, EntityType type, pi pos_, pi tileSize_ = pi(1, 1));
	virtual ~Entity();

#ifdef CLIENT
	virtual void draw(sf::RenderWindow *window, GameState *state);
#endif
	virtual void update(GameState *state) = 0;
	virtual bool isAlive() = 0;

	virtual pi getCollisionPos() const;
	const virtual pi &getPos() const;
	const virtual pf getPosWorld() const; // Centered assuming tileSize = pi(1, 1)
	pf getPosWorldCentered() const; // Centered using variable tileSize
	virtual void setPos(const pi &pos_);
	void moveBy(const pi &disp);

	//virtual void collide(Entity *other) = 0;
protected:
#ifdef CLIENT
	void setBoxTexture(TEXTURE texture);
	void setBoxSize();
	void setBoxSize(pf size);
	//void setBoxSize(int x, int y);
	void setBoxTint(sf::Color col);
#endif
private:
public:
	bool canWalk = false;
	const pi tileSize;
	const EntityType type;
	const EntitySpecies spec;
protected:
	pi pos;
protected:
#ifdef CLIENT
	sf::RectangleShape box;
#endif
private:
};


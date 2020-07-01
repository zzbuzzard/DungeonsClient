#include "pch.h"
#include "Entity.h"

#include "AllEnemies.h"

#include "GameState.h"

bool speciesCombatStatsConstant[NUM_ENTITY_SPECIES] = {
	false, // Player
	true,
	true,
	true,

	true,
	true,
	true,
	true,

	false, // Cursed cactus
	true,
	true,
	true,

	true,
	true,
	true,
	true,

	true,
	true, // Ent - used to be false, but now 1 phase only
	true,
	false, // Mega muncher

	true,
};


Entity::~Entity() {}

// Horrific. Yuck. But it works.
Entity* spawnEntityID(EntitySpecies spec, ID_t ID, const pi &pos) {
	switch (spec) {
	case E_ENEMY1:
		return new Enemy1(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_ENEMY2:
		return new Enemy2(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_ICE_ENEMY:
		return new EIceEnemy(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_MUNCHER:
		return new EMuncher(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_EYE_SPORE:
		return new EEyeSpore(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_SAND_GOLEM:
		return new ESandGolem(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_MUMMY_BOSS:
		return new EMummyBoss(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_FIRE_MAGE:
		return new EFireMage(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_SLIME:
		return new ESlime(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_ARCHER:
		return new EArcher(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_SCORPION:
		return new EScorpion(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_CURSED_CACTUS:
		return new ECursedCactus(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_DARK_MUNCHER:
		return new EDarkMuncher(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_ELF_ARCHER:
		return new EElfArcher(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_ELF_KNIGHT:
		return new EElfKnight(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_CURSED_ELF_KNIGHT:
		return new ECursedElfKnight(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_LEAFLING:
		return new ELeafling(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
	case E_ENT:
		return new EEnt(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
	case E_SPIDER:
		return new ESpider(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	case E_MEGA_MUNCHER:
		return new EMegaMuncher(pos
#ifdef CLIENT
			, ID);
#else
			);
#endif
		break;
	}
	cout << "Failed to spawn enemy, type = " << (int)spec << endl;
	return nullptr;
}

EntitySpecies randomEntityFromVector(const spawnVector &vec, int a, int b) {
	if (b == -1) b = vec.size() - 1;
	float t = 0;
	for (int i = a; i <= b; i++) {
		t += vec[i].second;
	}

	float x = util::rand(0, t);
	for (int i = a; i <= b; i++) {
		if (vec[i].second >= x) return vec[i].first;
		x -= vec[i].second;
	}
	cout << "Failed to spawn from vector" << endl;
	return vec[b].first;
}

Entity::Entity(EntitySpecies spec_, EntityType type_, pi pos_, pi tileSize_)
	: type(type_), spec(spec_), pos(pos_), tileSize(tileSize_) {
}

#ifdef CLIENT
#include "Util.h"
void Entity::draw(sf::RenderWindow *window, GameState *state) {
	if (type == ET_NO_BOX) return;
	box.setPosition(this->getPosWorld());
	if (state->isInView(getCollisionPos()))
		window->draw(box);
}

void Entity::setBoxTexture(TEXTURE texture) {
	box.setTexture(&textures[texture]);
}

void Entity::setBoxSize() {
	sf::Vector2f size = sf::Vector2f(PIXEL_PER_UNIT * tileSize.x, PIXEL_PER_UNIT * tileSize.y);
	box.setSize(size);
	box.setOrigin(sf::Vector2f(PIXEL_PER_UNIT, PIXEL_PER_UNIT) * 0.5f);
	//box.setOrigin(box.getSize()*0.5f);
}
void Entity::setBoxSize(sf::Vector2f size) {
	box.setSize(size);
	box.setOrigin(box.getSize() * 0.5f);
}

//void Entity::setBoxSize(int x, int y) {
//	setBoxSize(sf::Vector2f(x, y));
//}

void Entity::setBoxTint(sf::Color col) {
	box.setFillColor(col);
}
#endif 

const pi &Entity::getPos() const {
	return pos;
}

pi Entity::getCollisionPos() const {
	return pos;
}

const sf::Vector2f Entity::getPosWorld() const {
	return (float)PIXEL_PER_UNIT*(sf::Vector2f)pos;
}

sf::Vector2f Entity::getPosWorldCentered() const {
	pf x = getPosWorld();
	return x + pf(-0.5f + tileSize.x * 0.5f, -0.5f + tileSize.y * 0.5f) * (float)PIXEL_PER_UNIT;
}


void Entity::setPos(const pi &pos_) {
	pos = pos_;
}

void Entity::moveBy(const pi &disp) {
	pos = pos + disp;
}

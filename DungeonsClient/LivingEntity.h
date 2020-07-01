#pragma once
#include "Entity.h"
#include "Media.h"
#include <vector>

#ifdef CLIENT
#include <deque>
const float lifeBarHeight = 15.0f, lifeBarWidth = 70.0f;

// We have our list of moves; places that we should be at.
// Hopefully, they will all be very close together.
// What happens: 
//  If we are moving then continue moving.
//  When we reach a point where we can make a decision, get the direction towards where we want to go and move there.
//  Set our speed EVERY UPDATE

//struct Move {
//	pi pos;
//	Dir dir;
//	float moved;
//	Move(pi pos_, Dir dir_, float moved_)
//		: pos(pos_), dir(dir_), moved(moved_) {
//	}
//};
#endif

class GameState;

// Players:
//  Movement is manual
//  Target selection is manual
//  Target selection isnt capped at 1

// Players:
//  Shot damage
//  Crit chance
//  Range
//  Shot texture

// I am attacking an enemy in a group. The enemy is attacking another player.

//  1) I sent the server a request to target this enemy
//  2) The server receives this at 0.1s, and we start firing on the server side
//  3) At 1 second, we shoot, and deal 5 damage.
//  4) At 1.1 second, the server lets us shoot, and deal 5 damage.
//  5) At 1.2 second, we receive the server update and set the life.

// The way things will work
//  Each player login has a unique ID.
//  In the CLIENT HELLO, the client must log in.
//  If successful, we create a player for them etc. We also retrieve their inventory, equipment, level and stats.
//  The client is then told all of this in a big, targeted information bundle.
//  As these requests are asnychronous, we maintain a queue of returned results and the ID they apply to.

// The client knows their own stats.
// There are two approaches for other players
//  1) Send information every time another player shoots
//      advantage: simple, can sync critical hits with everyone, inventory changes immediate
//      disadvantage: lots of packets if many people are shooting fast, changes in ping mean wont be perfect
//  2) Send stats and then just send stat and target changes out
//      advantage: low latency, perfect shootin, 
//      disadvantage: cant show crits (fuck crits anyway tho), complicated to implement

// I'll go with no.2 for latency reasons (don't really want to process that many shoot events)

// We need to know, for all players, their current
//  1) Damage
//  2) Reload rate
//  3) Bullet texture
// Aka their combatStats

// The enemy
//  The enemy will be attacking someone
//  Their behaviour could be strange; attacking multiple people, or spawning area damage things
//  Same goes for players 

// Download speed is normally on the magnitude of MB/S
// Upload speed is around 5000 KB/S
// So even the server's 30kb/s * 30 people = 900KB/S is feasible.


// Combat stats should be everything the client needs to simulate an entity
//  walking around in the world, attacking, and being attacked.

struct CombatStats {
	damage_t damage = 0;
	float reloadTime = 10;
	range_t range = 0;

	float spd = 0.0f;
	def_t def = 0;

	TEXTURE t = T_NONE;
	CombatStats(damage_t damage_, float reloadTime_, range_t range_, TEXTURE t_, float spd_, def_t def_) : 
		damage(damage_), reloadTime(reloadTime_), range(range_), t(t_), spd(spd_), def(def_) {
	}
	CombatStats() {}
};

class LivingEntity : public Entity
{
// Public functions
public:
	LivingEntity(EntitySpecies spec, EntityType type, pi pos_, life_t life, ID_t id, pi tileSize_ = pi(1, 1));

	void update(GameState *state);
	virtual void handleTarget(ID_t id);
	void popTargets(target_count_t maxTargets);
	virtual void handleDirectionInput(Dir d, GameState *state);

	void setLife(life_t life);
#ifdef CLIENT
	void draw(sf::RenderWindow *window, GameState *state);
	void resizeLifebar();

	const void *handleUpdate(const void *data);
#endif
	const life_t &getLife();

	bool isAlive(); // Client always says yes, server knows best
	ID_t getID();

	pi getCollisionPos() const;
	const pf getPosWorld() const;
	virtual void takeDamage(damage_t dmg, ID_t damageDealer);
	// TODO: Currently not overriden by enemy. Do we even want pierce damage for enemies?
	virtual void takeDamagePierce(damage_t dmg, ID_t damageDealer);
	damage_t getDamageOf(damage_t dmg);

// Private functions
private:
	void moveNormally();

// Public variables
public:
	const ID_t ID;
	CombatStats combatStats;
	std::vector<ID_t> targets;

	Dir currentMovement;
	float moved = 0;

	life_t maxLife;

#ifdef CLIENT
	bool exists = true;
	bool local = false;
	bool isTargetted = false;
	bool isSpecialTargetted = false;
#endif

// Protected variables
protected:
	life_t lifeOn;

	float reloadOn = 0.0f;

// Private variables
private:
#ifdef CLIENT
	sf::RectangleShape lifebarbox, lifebar;

	sf::RectangleShape targetCircle, specialTargetCircle;

	bool hasMovedBefore = false;

	// At any given time,
	// We are moving towards positions[0]
	//std::deque<Move> positions;
	std::deque<pi> positions;
#endif
};


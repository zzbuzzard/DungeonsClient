#include "pch.h"
#include "ESlime.h"

static const CombatStats stats(20, 0.8f, 6, T_GOO_BALL, 1.5f, 4);
static const int life = 180;
static const int triggerRangee = 20;

#ifdef CLIENT
ESlime::ESlime(pi pos_, ID_t id)
	: Enemy(E_SLIME, pos_, life, id) {

	combatStats = stats;
	setBoxSize();
	setBoxTexture(T_SLIME);
}
#else
static const LootList Loot = { itemProb(I_SLIME_ESSENCE, 0.005f), itemProb(I_SLIME_FACEMASK, 0.008f) };
static const int xp = 150;

ESlime::ESlime(pi pos_)
	: Enemy(E_SLIME, pos_, life, triggerRangee, xp) {
	combatStats = stats;
	loot = &Loot;

}

#include "GameState.h"
#include "TopLayerTile.h"

void ESlime::update(GameState *state) {
	if (currentMovement == D_NONE) {
		targetClosestPlayer(state);
		moveTowardsPlayer(state);

		if (currentMovement != D_NONE) {
			auto vec = util::borderPoints(getCollisionPos(), tileSize, 1);
			state->sendTLtilesPacket(TLTileType::SPIKE_FLOOR, vec);
		}
	}
	Enemy::update(state);
}
#endif
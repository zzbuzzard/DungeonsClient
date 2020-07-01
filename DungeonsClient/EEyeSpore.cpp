#include "pch.h"
#include "EEyeSpore.h"
#include "Player.h"
#include "GameState.h"
#include "State.h"

const static CombatStats eyeSporeStats(20, 1.0f, 9, T_GOO_BALL, 0.0f, 0);
const static int eyeSporeLife = 1000;
const static int eyeSporeTriggerRange = 10;

#ifdef CLIENT
EEyeSpore::EEyeSpore(pi pos_, ID_t id)
	: Enemy(E_EYE_SPORE, pos_, eyeSporeLife, id) {

	combatStats = eyeSporeStats;
	setBoxSize();
	setBoxTexture(T_EYE_SPORE);
}
#else
const static LootList eyeSporeLoot = { itemProb(I_RECOVERY_CHARM, 0.0025f), itemProb(I_PLANT_ARMOUR, 0.01f) };

EEyeSpore::EEyeSpore(pi pos_)
	: Enemy(E_EYE_SPORE, pos_, eyeSporeLife, eyeSporeTriggerRange, 250) {
	combatStats = eyeSporeStats;
	loot = &eyeSporeLoot;
}


const static float retargetTime = 1.0f;
void EEyeSpore::update(GameState *state) {
	t -= deltaTime;
	if (t <= 0) {
		t = retargetTime;
		targetClosestPlayers(state, 2);
	}
	Enemy::update(state);
}
#endif
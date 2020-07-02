#include "pch.h"
#include "DisplayTileEntity.h"

DisplayTileEntity::DisplayTileEntity(pi pos_, TEXTURE texture) : Entity(E_NONE, ET_NONE, pos_) {
	setBoxSize();
	setBoxTexture(texture);

	canWalk = true;
	alive = true;
}

void DisplayTileEntity::update(GameState *state) {
}

bool DisplayTileEntity::isAlive() {
	return alive;
}

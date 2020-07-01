#include "pch.h"
#include "LootUI.h"
#include "Item.h"
#include "GameState.h"

const float
	spacing = 20,
	buttonSize = 120,
	yOffset = -150,
	FG_SIZE_MUL = 0.8f;

LootUI::LootUI(PlayerInfo *pInfo_) : pInfo(pInfo_) {
}

void LootUI::setLootUI(pi pos) {
	currentPos = pos;

	auto itemList = pInfo->getLootAt(pos);
	if (itemList == nullptr || itemList->size() == 0) {
		visible = false;
		return;
	}

	visible = true;

	// Plan : Set all buttons positions

	pf tilePos = (pf)pos * (float)PIXEL_PER_UNIT;

	itemButtons.clear();
	int N = itemList->size();

	float totalX = N * (buttonSize + spacing) + spacing;

	float yPos = tilePos.y + yOffset;
	float xPos = tilePos.x - totalX / 2 + spacing + buttonSize / 2;

	bg.setSize(pf(totalX, 2 * spacing + buttonSize));
	bg.setOrigin(bg.getSize() * 0.5f); // center
	
	bg.setPosition(pf(tilePos.x, yPos));

	sf::Color tintCol(200, 200, 200);

	for (int i = 0; i < itemList->size(); i++) {
		ITEM item = (*itemList)[i];

		Button b;

		b.setForegroundSize(pf(buttonSize, buttonSize) * FG_SIZE_MUL);
		b.setForegroundTexture(itemTypes[item]->texture);
		b.enableForeground();

		b.setTint(tintCol);
		b.setSize(pf(buttonSize, buttonSize));
		b.setPos(pf(xPos, yPos));

		itemButtons.push_back(b);

		xPos += spacing + buttonSize;
	}
}

void LootUI::draw(sf::RenderWindow *window) {
	if (visible) {
		window->draw(bg);
		for (auto &b : itemButtons) {
			b.draw(window);
		}
	}
}

void LootUI::update(GameState *state) {
	if (state->getLocalPlayerPos() != currentPos) {
		setLootUI(state->getLocalPlayerPos());
	}

	if (visible && mouseClicked) {
		for (int i = 0; i < itemButtons.size(); i++) {
			if (itemButtons[i].mouseIsOver(state)) {
				pInfo->pickUp(state->getLocalPlayerPos(), i);
				setLootUI(state->getLocalPlayerPos());
				state->lootChanged = true;
				break;
			}
		}
	}
}

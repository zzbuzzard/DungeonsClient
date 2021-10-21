#include "pch.h"
#include "GameMenu.h"
#include "GameState.h"
#include "Util.h"
#include "Packet.h"
#include "Controller.h"
#include "ConnectionManager.h"

#include <iostream>
using std::cout;
using std::endl;

#define BUTTON_SIZE 90.0f
//#define VIEW_HEIGHT 1000.0f

#define FG_MUL 0.8f

// Width of the game menu in *game units*
#define WIDTH 460.0f

// Drag n drop logic :
//  Variable DRAG and bool equipWasDragged are set when we click.
//  Each button becomes invisible if its the one getting dragged.
//  When we release, we check everyone to see if the mouse is over them.

//static pf statsTextOffset(WIDTH/2 - 54.0f, 288);
static float statsTextWidth = 380.0f;
static pf statsTextOffset((WIDTH-statsTextWidth)/2.0f, 165);

const static sf::Color tintCol(200, 200, 200);
const static sf::Color loadoutSelectedTintCol(220, 220, 0);

GameMenu::GameMenu() 
	: statsUI(statsTextWidth)
{
	cachedStats.stats[0] = -1;

	//statsText.setFont(g_pixelFont);
	//statsText.setCharacterSize(50);
	//statsText.setFillColor(sf::Color::Black);

	panel.setSize(sf::Vector2f(WIDTH, VIEW_HEIGHT));
	panel.setPosition(sf::Vector2f(0, 0));

	dragBox.setSize(sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE) * FG_MUL);
	dragBox.setOrigin(dragBox.getSize() * 0.5f);

	float used_x = BUTTON_SIZE * INV_ALONG;
	sx = (WIDTH - used_x) / (INV_ALONG + 1);

	// the screen is 500 game units vertically

	float topLeftX = sx, topLeftY = VIEW_HEIGHT - (sx + BUTTON_SIZE) * INV_UP;

	sf::Vector2f btnOffset = sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE)*0.5f;

	for (int y = 0; y < INV_UP; y++) {
		for (int x = 0; x < INV_ALONG; x++) {
			float X = topLeftX + x * (sx + BUTTON_SIZE);
			float Y = topLeftY + y * (sx + BUTTON_SIZE);

			inventoryButtons[y*INV_ALONG + x].setSize(sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE));
			inventoryButtons[y*INV_ALONG + x].setTint(tintCol);
			//inventoryButtons[y*INV_ALONG + x].setPos();
			invButtonPos[y*INV_ALONG + x] = sf::Vector2f(X, Y) + btnOffset;

			inventoryButtons[y*INV_ALONG + x].setForegroundSize(sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE) * FG_MUL);
		}
	}

	float middle_y = VIEW_HEIGHT * 0.5f - BUTTON_SIZE * 0.5f + 50.0f;

	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		equipmentButtons[i].setSize(sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE));
		equipmentButtons[i].setTint(tintCol);
		//equipmentButtons[i].setPos();
		eqButtonPos[i] = sf::Vector2f(sx + i * (sx + BUTTON_SIZE), middle_y) + btnOffset;

		equipmentButtons[i].setForegroundSize(sf::Vector2f(BUTTON_SIZE, BUTTON_SIZE) * FG_MUL);
	}
	eqButtonPos[4] = sf::Vector2f(sx + 3 * (sx + BUTTON_SIZE), middle_y - sx - BUTTON_SIZE) + btnOffset;
	//equipmentButtons[4].setPos(sf::Vector2f(sx + 3 * (sx + BUTTON_SIZE), middle_y - sx - BUTTON_SIZE));

	for (int i = 0; i < LOADOUTS; i++) {
		loadoutButtons[i].setSize(pf(BUTTON_SIZE, BUTTON_SIZE));
		loadoutButtons[i].setTint(tintCol);
		loadoutButtons[i].setText(std::to_string(i+1));
		loadoutButtons[i].setFontSize(60);

		loadoutButtonPos[i] = pf(sx + i * (sx + BUTTON_SIZE), middle_y - sx - BUTTON_SIZE) + btnOffset;

		for (int j = 0; j < NUM_EQUIP_LOCS; j++) {
			loadouts[i][j] = I_NONE;
		}
	}
}

// panel goes in top left

// We need to move em to their actual positions
// Actual pos = offset + start pos

bool hPressed = false;

void GameMenu::update(GameState *state) {
	int loadoutLoadRequest = getLoadoutKeyboardPress();
	if (loadoutLoadRequest != -1) {
		setLoadout(state, loadoutLoadRequest);
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::H)) {
		if (!hPressed && !isTyping) {
			isVisible = !isVisible;

			if (!isVisible) {
				isDragging = false;
			}
		}
		hPressed = true;
	}
	else hPressed = false;
	chat.update(state);
	if (!isVisible) return;

	xpBar.update(&state->pInfo);

	const int line = 8;

	Stats stats = state->pInfo.getOverallStats();

	if (stats != cachedStats) {
		cachedStats = stats;
		statsUI.updateDisplay(stats);
	}

	if (mouseClicked) {
		for (int i = 0; i < INV_SIZE; i++) {
			ITEM item = state->pInfo.inv.items[i];
			if (item != I_NONE && inventoryButtons[i].mouseIsOver(state)) {
				isDragging = true;
				draggingInd = i;
				equipDragged = false;

				dragBox.setTexture(&textures[itemTypes[item]->texture]);
				break;
			}
		}

		for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
			ITEM item = state->pInfo.inv.equips[i];

			if (item != I_NONE && equipmentButtons[i].mouseIsOver(state)) {
				isDragging = true;
				draggingInd = i;
				equipDragged = true;

				dragBox.setTexture(&textures[itemTypes[item]->texture]);
				break;
			}
		}

		if (!isDragging) {
			for (int i = 0; i < LOADOUTS; i++) {
				if (loadoutButtons[i].mouseIsOver(state)) {
					setLoadout(state, i);
					break;
				}
			}
		}

		// If dungeon
		if (state->gameStateType != GameStateType::Dungeon) {
			if (draggingInd == (int)Equipment::SPECIAL && equipDragged) {
				isDragging = false;
			}
		}
	}

	if (isDragging && mouseReleased) {
		int ind = -1; // get clicked index and if it was an equip
		bool eq;

		for (int i = 0; i < INV_SIZE; i++) {
			if (inventoryButtons[i].mouseIsOver(state)) {
				ind = i;
				eq = false;
				break;
			}
		}

		for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
			if (equipmentButtons[i].mouseIsOver(state)) {
				ind = i;
				eq = true;
				break;
			}
		}

		// Dragged into nowhere
		// TODO : DROP IF X > PANEL
		if (ind == -1) {
			if (!equipDragged) {
				pf pos = state->getMousePosWorld();
				pf tl = state->getTopLeftWorld();

				float x = pos.x - tl.x;
				if (x >= WIDTH) {
					state->pInfo.dropItem(draggingInd);
				}
			}
		}
		else {
			// We dragged an equip
			if (equipDragged) {
				// Equip -> Equip is nothing (either to myself or to somewhere i cant go)
				if (eq == false) {
					state->pInfo.inv.unequip(draggingInd, ind);
				}
			}
			// We dragged an item
			else {
				// Attempted to equip
				if (eq) {
					state->pInfo.inv.equip(draggingInd);
				}
				// Just moving about in inventory
				else {
					state->pInfo.inv.move(draggingInd, ind);
				}
			}
		}

		isDragging = false;
	}

	// Set hover ui's display
	ITEM it = I_NONE;
	for (int i = 0; i < INV_SIZE; i++) {
		if (inventoryButtons[i].mouseIsOver(state)) {
			it = state->pInfo.inv.items[i];
			break;
		}
	}

	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		if (equipmentButtons[i].mouseIsOver(state)) {
			it = state->pInfo.inv.equips[i];
			break;
		}
	}
	hoverUI.display(it);
	hoverUI.update(state);
}

void GameMenu::draw(GameState *state, sf::RenderWindow *window) {
	if (!isVisible) {
		chat.bottomLeft = state->getTopLeftWorld() + pf(0, VIEW_HEIGHT);
		chat.draw(window);
		return;
	}

	offset = state->getTopLeftWorld();
	panel.setPosition(offset);
	window->draw(panel);

	const Inventory &inv = state->pInfo.inv;

	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		bool hasFG = inv.equips[i] != I_NONE;
		if (isDragging) {
			if (equipDragged && draggingInd == i) hasFG = false;
		}

		if (!hasFG) {
			equipmentButtons[i].disableForeground();
		}
		else {
			equipmentButtons[i].setForegroundTexture(itemTypes[inv.equips[i]]->texture);
			equipmentButtons[i].enableForeground();
		}

		equipmentButtons[i].setPos(offset + eqButtonPos[i]);
		//equipmentButtons[i].move(offset);
		equipmentButtons[i].draw(window);
		//equipmentButtons[i].move(-offset);
	}

	for (int i = 0; i < LOADOUTS; i++) {
		if (i == currentLoadout)
			loadoutButtons[i].setTint(loadoutSelectedTintCol);
		else
			loadoutButtons[i].setTint(tintCol);

		loadoutButtons[i].setPos(offset + loadoutButtonPos[i]);
		loadoutButtons[i].draw(window);
	}

	for (int i = 0; i < INV_SIZE; i++) {
		bool hasFG = inv.items[i] != I_NONE;
		if (isDragging) {
			if (!equipDragged && draggingInd == i) hasFG = false;
		}

		if (!hasFG) {
			inventoryButtons[i].disableForeground();
		}
		else {
			inventoryButtons[i].setForegroundTexture(itemTypes[inv.items[i]]->texture);
			inventoryButtons[i].enableForeground();
		}

		inventoryButtons[i].setPos(offset + invButtonPos[i]);
		//inventoryButtons[i].move(offset);
		inventoryButtons[i].draw(window);
		//inventoryButtons[i].move(-offset);
	}

	statsUI.setPos(offset + statsTextOffset);
	statsUI.draw(state, window);

	//statsText.setPosition(statsTextOffset + offset);
	//window->draw(statsText);

	xpBar.setPos(offset, WIDTH);
	xpBar.draw(window);

	chat.bottomLeft = state->getTopLeftWorld() + pf(WIDTH, VIEW_HEIGHT);
	chat.draw(window);

	if (isDragging) {
		dragBox.setPosition(state->getMousePosWorld());
		window->draw(dragBox);
	}
	else {
		hoverUI.draw(state, window);
	}
}


// If we press 2, and we're not on 2,
//  1) Unequip each thing that isnt the same in 2
//  2) For each slot that is empty, if its not empty in 2, loop through inv and equip
//    [if we can't find it, set savedEquips to none]

void GameMenu::setLoadout(GameState *state, int n) {
	if (n == currentLoadout)
		return;

	cout << "Setting loudout to " << n << endl;

	Inventory &inv = state->pInfo.inv;

	// Set current loadout
	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		loadouts[currentLoadout][i] = inv.equips[i];
	}

	int equipIndices[NUM_EQUIP_LOCS];

	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		equipIndices[i] = -1;

		ITEM goal = loadouts[n][i];
		if (goal != inv.equips[i] && goal != I_NONE) {
			for (int j = 0; j < INV_SIZE; j++) {
				if (inv.items[j] == goal) {
					equipIndices[i] = j;
					break;
				}
			}
		}
	}

	// Swap stuff out first
	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		if (equipIndices[i] != -1) {
			if (inv.equips[i] == I_NONE)
				inv.equip(equipIndices[i]);
			else
				inv.unequip(i, equipIndices[i]);
		}
	}
	// Now handle the -1s:
	//  If they're equal then leave it
	//  If the goal is empty and it's not empty then unequip to first free [or don't if none free]
	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		if (equipIndices[i] == -1) {
			ITEM goal = loadouts[n][i];

			// It's fine, they're equal
			if (inv.equips[i] == goal) continue;

			// If we wanted nothing but we have something, unequip the something [if possible]
			if (goal == I_NONE && inv.equips[i] != I_NONE) {
				int ind = inv.getFirstFree();
				if (ind != -1)
					inv.unequip(i, ind);
				
				// otherwise that just sucks, no room to unequip it

				continue;
			}

			// If we reach here, goal != I_NONE
			// So goal is some item, and inv.equips is not that item
			// As its -1, it means we were unable to find the item that we wanted...
			// So unequip if possible
			int ind = inv.getFirstFree();
			if (ind != -1)
				inv.unequip(i, ind);
		}
	}

	currentLoadout = n;
}



int GameMenu::getLoadoutKeyboardPress() {
	static bool pressed[LOADOUTS] = { false, false, false };
	static sf::Keyboard::Key keys[LOADOUTS] = { sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3 };

	int n = -1;

	for (int i = 0; i < LOADOUTS; i++) {
		if (sf::Keyboard::isKeyPressed(keys[i])) {
			if (!pressed[i])
				n = i;
			pressed[i] = true;
		}
		else {
			pressed[i] = false;
		}
	}
	return n;
}

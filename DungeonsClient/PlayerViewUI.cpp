#include "pch.h"
#include "PlayerViewUI.h"
#include "Player.h"
#include "GameState.h"

const float
	PlayerViewUI::textHeight = 60.0f,
	PlayerViewUI::playerImgSize = 160.0f,
	PlayerViewUI::border = 5.0f,
	PlayerViewUI::itemButtonBorder = 15.0f;

const int PlayerViewUI::textFontSize = 60;

sf::Color pvu_backgroundColour(220, 220, 220);
sf::Color pvu_itemBackgroundColour(180, 180, 180);

// panelPos, playerImgPos, gearPos[NUM_EQUIP_LOCS], nameTextPos

PlayerViewUI::PlayerViewUI() {
	const float FG_MUL = 0.8f;

	playerBox.setSize(pf(playerImgSize, playerImgSize));
	player.setSize(pf(playerImgSize, playerImgSize) * FG_MUL);
	player.setOrigin(- (playerBox.getSize() - player.getSize()) * 0.5f);

	playerBox.setFillColor(pvu_itemBackgroundColour);

	float ibSize = playerImgSize - textHeight;

	float panelWidth = 2*itemButtonBorder + playerImgSize + (itemButtonBorder + ibSize) * NUM_EQUIP_LOCS;
	float panelHeight = itemButtonBorder * 2 + playerImgSize;

	panel.setSize(pf(panelWidth, panelHeight));
	panel.setFillColor(pvu_backgroundColour);

	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		gearBoxes[i].setSize(pf(ibSize, ibSize));
		gearBoxes[i].setFillColor(pvu_itemBackgroundColour);

		gearImages[i].setSize(pf(ibSize, ibSize) * FG_MUL);
		gearImages[i].setOrigin(-(gearBoxes[i].getSize() - gearImages[i].getSize()) * 0.5f);
	}

	nameText.setCharacterSize(textFontSize);
	nameText.setFont(g_pixelFont);
	nameText.setFillColor(sf::Color::Black);

	nameText.setString("Player");
	auto r = nameText.getGlobalBounds();
	nameText.setOrigin(r.left, r.top);
}

void PlayerViewUI::setDisplay(Player *p) {
	isVisible = true;

	nameText.setString(p->name + ", Level " + std::to_string(p->level));
	player.setTexture(&textures[T_PLAYER]); // TODO : SKINS

	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		ITEM item = p->equips[i];
		if (item == I_NONE) {
			gearImages[i].setFillColor(sf::Color::Transparent);
		}
		else {
			gearImages[i].setFillColor(sf::Color::White);

			TEXTURE t = itemTypes[item]->texture;
			gearImages[i].setTexture(&textures[t]);
		}
	}
}

void PlayerViewUI::close() {
	isVisible = false;
}

// pos is the position of the panel
void PlayerViewUI::setPos(pf pos) {
	panel.setPosition(pos);

	pf p = pos + pf(itemButtonBorder, itemButtonBorder);
	player.setPosition(p);
	playerBox.setPosition(p);

	float x = p.x + playerImgSize + itemButtonBorder;
	float y = p.y + textHeight;

	nameText.setPosition(pf(x, p.y));

	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		gearImages[i].setPosition(pf(x, y));
		gearBoxes[i].setPosition(pf(x, y));
		x += gearBoxes[i].getSize().x + itemButtonBorder;
	}
}

void PlayerViewUI::draw(GameState *state, sf::RenderWindow *window) {
	if (isVisible) {
		pf BR = state->getBottomRightWorld();
		BR -= pf(border, border);
		BR -= panel.getSize();

		setPos(BR);

		window->draw(panel);
		
		window->draw(playerBox);
		window->draw(player);

		window->draw(nameText);

		for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
			window->draw(gearBoxes[i]);
			window->draw(gearImages[i]);
		}
	}
}


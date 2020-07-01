#include "pch.h"
#include "ItemHoverUI.h"
#include "GameState.h"
#include "Stats.h"

const sf::Vector2f itemHoverStatsTextOffset(8.0f, 40.0f),
itemHoverTitleTextOffset(5.0f, 0);

ItemHoverUI::ItemHoverUI() {
	panel.setFillColor(sf::Color(180, 180, 180));

	titleText.setFont(g_pixelFont);
	titleText.setCharacterSize(55);
	titleText.setFillColor(sf::Color::Black);
	titleText.setOrigin(sf::Vector2f(0, 15));

	statsText.setFont(g_pixelFont);
	statsText.setCharacterSize(40);
	statsText.setFillColor(sf::Color::Black);
}

void ItemHoverUI::update(GameState *state) {
}

void ItemHoverUI::draw(GameState *state, sf::RenderWindow *window) {
	if (isVisible) {
		setPos(state->getMousePosWorld());

		window->draw(panel);
		window->draw(titleText);
		window->draw(statsText);
	}
}

void ItemHoverUI::display(ITEM itemType) {
	if (itemType == I_NONE) {
		isVisible = false;
		return;
	}
	isVisible = true;

	if (itemType == currentlyDisplaying) return;
	currentlyDisplaying = itemType;

	Item *item = itemTypes[itemType];

	titleText.setString(item->name);

	std::string t = "";
	for (int i = 0; i < NUM_STATS; i++) {
		int statValue = item->bonus.stats[i];

		if (statValue != 0)
			t += statNames[i] + " " + (statValue > 0 ? "+" : "") + std::to_string(statValue) + "\n";
	}

	if (item->location == Equipment::WEAPON) {
		Weapon *w = (Weapon*)item;
		t += "Damage: " + std::to_string(w->damage) + "\n";
		float shots = 1 / w->reload;
		t += "Shots per second: " + util::floatToString(shots, 1) + "\n";

		t += "Range: " + std::to_string(w->range) + "\n";

		if (w->num_targets != 1) {
			t += "Number of targets: " + std::to_string(w->num_targets) + "\n";
		}
	}
	// Desc
	// MP cost
	if (item->location == Equipment::SPECIAL) {
		Special *s = (Special*)item;

		t += s->desc + "\n";
		t += "MP: " + std::to_string(s->mpCost) + "\n";
		t += "Range: " + std::to_string(s->range) + "\n";
		t += "Number of targets: " + std::to_string(s->num_targets) + "\n";
	}

	statsText.setString(t);

	float h = statsText.getGlobalBounds().height + 60.0f;
	float w = std::max(statsText.getGlobalBounds().width, titleText.getGlobalBounds().width) + 20.0f;
	panel.setSize(sf::Vector2f(w, h));
}

void ItemHoverUI::setPos(sf::Vector2f pos) {
	panel.setPosition(pos);
	titleText.setPosition(pos + itemHoverTitleTextOffset);
	statsText.setPosition(pos + itemHoverStatsTextOffset);
}


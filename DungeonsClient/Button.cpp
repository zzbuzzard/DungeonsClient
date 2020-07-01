#include "pch.h"
#include "Button.h"
#include "State.h"
#include "Util.h"

Button::Button() {
	text.setFont(g_pixelFont);
	text.setCharacterSize(60);
	text.setFillColor(sf::Color::Black);

	hasText = false;
	hasForeground = false;
}

void Button::setPos(pf pos_) {
	pos = pos_;
	box.setPosition(pos);
	fg.setPosition(pos);
	text.setPosition(pos);
}

//void Button::move(sf::Vector2f pos_) {
//	pos += pos_;
//	box.setPosition(pos);
//}

void Button::setSize(pf size_) {
	size = size_;
	box.setSize(size);
	box.setOrigin(box.getSize() * 0.5f);
}

//void Button::setOrigin(sf::Vector2f origin) {
//	box.setOrigin(origin);
//}

void Button::setTexture(TEXTURE texture) {
	box.setTexture(&textures[texture]);
}

void Button::setTint(sf::Color col) {
	box.setFillColor(col);
}

void Button::setText(std::string st) {
	if (st.size() > 0) {
		hasText = true;
		text.setString(st);
		util::centerSfText(text);
	}
	else hasText = false;
}

void Button::setFontSize(unsigned int size) {
	text.setCharacterSize(size);
}

bool Button::mouseIsOver(State *state) {
	sf::Vector2f m = state->getMousePosWorld();
	if (m.x > pos.x - size.x * 0.5f && m.x < pos.x + size.x * 0.5f) {
		if (m.y > pos.y - size.y * 0.5f && m.y < pos.y + size.y * 0.5f) {
			return true;
		}
	}
	return false;
}


void Button::draw(sf::RenderWindow *window) {
	window->draw(box);
	if (hasForeground) {
		enableForeground();
		window->draw(fg);
	}
	if (hasText) window->draw(text);
}

void Button::setForegroundSize(pf size_) {
	fg.setSize(size_);
	fg.setOrigin(fg.getSize() * 0.5f);
}

void Button::setForegroundTexture(TEXTURE texture) {
	if (&textures[texture] != fg.getTexture())
		fg.setTexture(&textures[texture]);
}

void Button::enableForeground() {
	hasForeground = true;
	
	//sf::Vector2f t = (size - fg.getSize()) * 0.5f;
	//fg.setPosition(t + box.getPosition() - box.getOrigin());
}

void Button::disableForeground() {
	hasForeground = false;
}

#include "pch.h"
#include "TextInput.h"
#include "State.h"
#include "Media.h"

// for debugging only
#include "Util.h"

const float caretWidth = 3.0f;
const float caretMoveTime = 0.1f;

TextInput::TextInput(int maxChars_) : maxChars(maxChars_) {
	rText.setFont(g_pixelFont);
	calculateSizes();
	recenter();
}

void TextInput::update(State *state) {
	if (caretMoveCool > 0) caretMoveCool -= deltaTime;

	if (isInFocus()) {
		if (typedChar != 0) {
			char c = typedChar;

			// Space, other normal characters
			if (c >= 32) {
				if (text.size() < maxChars) {
					//text.push_back(c);
					text.insert(text.begin() + caretPos, c);

					caretPos++;

					rText.setString(text);
					recenter();
				}
			}
			else {
				// TODO: Handle backspace, arrowkeys
				if (c == 8) {
					if (text.size() > 0 && caretPos > 0) {
						text.erase(text.begin() + caretPos - 1);

						caretPos--;

						//text.erase(text.end() - 1);
						rText.setString(text);
						recenter();
					}
				}
			}
		}

		if (caretMoveCool <= 0) {
			int disp = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) disp--;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) disp++;

			bool shortOne = movedCaret;

			if (disp != 0) {
				caretPos += disp;
				if (caretPos < 0) caretPos = 0;
				if (caretPos > text.size()) caretPos = text.size();

				movedCaret = true;
			}
			else movedCaret = false;

			caretMoveCool = caretMoveTime * (shortOne ? 0.25f : 1.0f);
		}
	}
}

void TextInput::draw(sf::RenderWindow *window) {
	if (isInFocus()) {
		caret.setPosition(rText.findCharacterPos(caretPos) + sf::Vector2f(0, caret.getSize().y));

		//auto rect = rText.getGlobalBounds();
		//caret.setPosition(sf::Vector2f(rect.left + rect.width, rText.getPosition().y));
		window->draw(caret);
	}

	window->draw(rText);
}

void TextInput::setPos(sf::Vector2f pos) {
	rText.setPosition(pos);
}

void TextInput::setFontSize(unsigned int size) {
	rText.setCharacterSize(size);
	calculateSizes();
}

void TextInput::calculateSizes() {
	rText.setString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!'\"#~][}{()");
	fixedHeight = rText.getLocalBounds().top;
	caret.setSize(sf::Vector2f(caretWidth, rText.getLocalBounds().height));
	rText.setString("");
}


void TextInput::setFontColor(sf::Color col) {
	rText.setFillColor(col);
	caret.setFillColor(col);
}

void TextInput::setOutlineColor(sf::Color col) {
	rText.setOutlineColor(col);
	caret.setOutlineColor(col);
}

void TextInput::setOutline(float w) {
	rText.setOutlineThickness(w);
	caret.setOutlineThickness(w);
}

//void TextInput::setFixedHeight(float h) {
//	fixedHeight = h;
//}

void TextInput::setFocus() {
	isTyping = true;
	currentTextInput = this;
}

void TextInput::loseFocus() {
	isTyping = false;
	currentTextInput = nullptr;
}


void TextInput::clear() {
	text = "";
	caretPos = 0;
	rText.setString(text);
	recenter();
}

std::string TextInput::getString() {
	return text;
}

void TextInput::setString(std::string string) {
	text = string;
	caretPos = text.size();
	rText.setString(text);
}


bool TextInput::isInFocus() {
	if (!isTyping) return false;
	return currentTextInput == this;
}

// Origin set to the top left of the text's bounding box
void TextInput::recenter() {
	auto textRect = rText.getLocalBounds();
	rText.setOrigin(textRect.left, fixedHeight); // textRect.top
}

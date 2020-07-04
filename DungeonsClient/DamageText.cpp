#include "pch.h"
#include "DamageText.h"
#include "State.h"
#include "LivingEntity.h"

static int damageFontSize = 100;
static sf::Color damageColor = sf::Color(200, 0, 0);
static sf::Color healColor = sf::Color::Green;
static sf::Color xpColor = sf::Color(21, 255, 0);

static sf::Color outline = sf::Color::Black;
static float outlineAmount = 2.0f;

static float damageTextTime = 0.2f;
static float damageTextSpd = -275.0f;

pf offset = sf::Vector2f(0, 10.0f);

DamageText::DamageText(pf pos_, int dmg, bool isXP_)
	: Entity(E_NONE, ET_NO_BOX, pi(0,0)), wpos(pos_ + offset), isXP(isXP_)
{
	canWalk = true;

	text.setFont(g_pixelFont);
	text.setOutlineColor(outline);
	text.setOutlineThickness(outlineAmount);
	text.setCharacterSize(damageFontSize);
	text.setStyle(sf::Text::Style::Bold);

	if (isXP) {
		text.setFillColor(xpColor);
		text.setString(std::to_string(dmg));
	}
	else {
		if (dmg > 0) {
			text.setFillColor(damageColor);
			text.setString(std::to_string(dmg));
		}
		else {
			text.setFillColor(healColor);
			text.setString(std::to_string(abs(dmg)));
		}
	}

	util::centerSfText(text);

	life = damageTextTime;
}

DamageText::DamageText(Entity *follow_, pf offset, int dmg, bool isXP_)
	: DamageText(offset, dmg, isXP_)
{
	follow = follow_;
}

void DamageText::init() {

}

void DamageText::update(GameState *state) {
	float mul = (isXP ? 0.5f : 1.0f);
	life -= deltaTime * mul;
	wpos.y += damageTextSpd * deltaTime * mul;

	if (follow != nullptr) {
		if (follow->type == ET_PLAYER || follow->type == ET_ENEMY) {
			if (!((LivingEntity*)follow)->isAlive()) {
				wpos += follow->getPosWorldCentered();
				follow = nullptr;
			}
		}
	}
}


bool DamageText::isAlive() {
	return life >= 0;
}


void DamageText::draw(sf::RenderWindow *window, GameState *state) {
	if (follow == nullptr)
		text.setPosition(wpos);
	else 
		text.setPosition(wpos + follow->getPosWorldCentered());
	window->draw(text);
}


pf DamageText::getPosWorldCentered() const {
	if (follow == nullptr)
		return wpos;
	else
		return wpos + follow->getPosWorldCentered();
}
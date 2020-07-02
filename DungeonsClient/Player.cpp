#include "pch.h"
#include "Player.h"
#include "State.h"
#include "Stats.h"
#include "DamageText.h"

//CombatStats defaultStats = CombatStats(10, 0.4f, 8, T_ARROW);
CombatStats noWeapon = CombatStats(0, 1.0f, 0, T_NONE, 1.0f, 0);

#ifdef CLIENT
#include "GameState.h"
Player::Player(pi pos_, ID_t id)
	: LivingEntity(E_PLAYER, ET_PLAYER, pos_, 100, id) {
	setBoxSize();
	setBoxTexture(T_PLAYER);
	combatStats = noWeapon;

	// TODO: Sort names out
	name = "Player " + std::to_string(id);

	manabar.setFillColor(sf::Color::Blue);
	manabarbox.setFillColor(sf::Color::Black);

	nameText.setFont(g_pixelFont);
	nameText.setFillColor(sf::Color::Black);
	nameText.setCharacterSize(40);
	nameText.setString(name);

	for (int i = 0; i < NUM_EQUIP_LOCS; i++) {
		equips[i] = I_NONE;
	}
}

float Player::jerkTime = -1;
//sf::Vector2f Player::jerkPos, Player::jerkingTowards;
pf Player::jerkMover, Player::jerkPos;
const float totalJerkTime = 0.2f;

void Player::jerk(pi pos_) {
	jerkPos = getPosWorld();

	jerkMover = (pf)pos_ * (float)PIXEL_PER_UNIT - jerkPos;
	jerkTime = totalJerkTime;

	float m = util::mag(jerkMover);
	float spd = m / totalJerkTime;

	jerkMover = jerkMover / m * spd;

	currentMovement = D_NONE;
	moved = 0;

	//jerkPos = getPosWorld();
	//jerkTime = 0.3f;
	//jerkingTowards = ;

	pos = pos_;
}

const float manaBarYGap = 47.5f, manaBarHeight = 15.0f, manaBarWidth = 70.0f;
sf::Color manabarMaxCol(0, 0, 255), manabarMinCol(255, 255, 255);

void Player::update(GameState *state) {
	if (local) {
		Stats st = state->pInfo.getOverallStats();
		combatStats.spd = st.getSpeed();
		maxLife = st.getHP();
		maxMP = st.getMP();
		if (lifeOn > maxLife) lifeOn = maxLife;
		if (mpOn > maxMP) mpOn = maxMP;
		if (jerkTime > 0) {
			pf disp = LivingEntity::getPosWorld() - jerkPos;
			float m = util::mag(disp);
			float spd = m / jerkTime;

			jerkTime -= deltaTime;
			jerkPos += disp / m * spd * deltaTime;
		}
	}

	if (TLdmgCool <= 0) {
		// INTENTIONALLY pos and not getCollisionPos()
		Entity *e = state->getTLTile(pos);
		if (e != nullptr) {
			if (e->type == ET_TOP_LAYER_TILE) {
				TopLayerTile *x = (TopLayerTile*)e;
				if (x->getData()->damaging) {
					DamageTileData *dtd = (DamageTileData*)x->getData();

					state->addEntity(new DamageText(this, pf(0, -0.5f * PIXEL_PER_UNIT), dtd->damage));
					TLdmgCool = dtd->damageCool;
				}
			}
		}
	}
	else TLdmgCool -= deltaTime;


	LivingEntity::update(state);

	resizeManaBar();
	float overlap = (lifeBarWidth - PIXEL_PER_UNIT) * tileSize.x;
	float x = -(PIXEL_PER_UNIT + overlap) / 2.0f;
	float y = (tileSize.y - 0.5f) * PIXEL_PER_UNIT + manaBarYGap;
	manabarbox.setPosition(getPosWorld() + sf::Vector2f(x, y));// +tileOffset);
	manabar.setPosition(getPosWorld() + sf::Vector2f(x, y));// +tileOffset);
}

const sf::Vector2f Player::getPosWorld() const {
	if (!local || jerkTime <= 0) return LivingEntity::getPosWorld();
	return jerkPos;
}

void Player::draw(sf::RenderWindow *window, GameState *state) {
	if (local && state->isDead) return; // When we are waiting to spawn we don't show up
	LivingEntity::draw(window, state);
	sf::Vector2f pos = getPosWorld();

	auto textRect = nameText.getLocalBounds();
	nameText.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);

	static const float nameTextYGap = 10.0f;

	nameText.setPosition(pos + pf(0, nameTextYGap + PIXEL_PER_UNIT * 0.5f));

	window->draw(nameText);
	window->draw(manabarbox);
	window->draw(manabar);
}

void Player::resizeManaBar() {
	manabar.setFillColor(util::colorLerp(manabarMinCol, manabarMaxCol, mpOn / (float)maxMP));

	manabarbox.setSize(sf::Vector2f(manaBarWidth * tileSize.x, manaBarHeight));
	manabarbox.setOrigin(sf::Vector2f(0, manabar.getSize().y*0.5f));
	manabar.setSize(sf::Vector2f(manaBarWidth * tileSize.x * mpOn / maxMP, manaBarHeight));
	manabar.setOrigin(sf::Vector2f(0, manabar.getSize().y*0.5f));
}


#else
#include "GameState.h"
ID_t Player::idd = 0;
Player::Player(pi pos_, sf::TcpSocket *sock)
	: LivingEntity(E_PLAYER, ET_PLAYER, pos_, 100, idd++), socket(sock) {
	combatStats = noWeapon;
}

void Player::appendBaseInv(sf::Packet &packet) {
	for (int i = 0; i < NUM_STATS; i++)
		packet.append(&pInfo.getBaseStats().stats[i], sizeof(stat_t));
	pInfo.inv.appendToPacket(packet);
}

void Player::update(GameState *state) {
	Stats st = pInfo.getOverallStats();
	combatStats.spd = st.getSpeed();

	maxLife = st.getHP();
	maxMP = st.getMP();

	if (TLdmgCool <= 0) {
		// INTENTIONALLY pos and not getCollisionPos()
		Entity *e = state->getTLTile(pos);
		if (e != nullptr) {
			if (e->type == ET_TOP_LAYER_TILE) {
				TopLayerTile *x = (TopLayerTile*)e;
				if (x->getData()->damaging) {
					DamageTileData *dtd = (DamageTileData*)x->getData();

					// TODO: Check dtd's lifetime==-1, cast to TimedTopLayerTile
					if (dtd->lifetime != -1 && ((TimedTopLayerTile*)x)->livedFor < 0.1f) {
						cout << "Sparing a player because the tiles been alive < 0.1s" << endl;
					}
					else {
						takeDamagePierce(dtd->damage, -1);
						TLdmgCool = dtd->damageCool;
					}
				}
			}
		}
	}
	else TLdmgCool -= deltaTime;

	timeToNextHeal -= deltaTime;
	while (timeToNextHeal <= 0) {
		timeToNextHeal += st.getRecTime();
		lifeOn++;
	}
	if (lifeOn > maxLife) lifeOn = maxLife;

	timeToNextMPHeal -= deltaTime;
	while (timeToNextMPHeal <= 0) {
		timeToNextMPHeal += st.getMpRecTime();
		mpOn++;
	}

	if (mpOn > maxMP) mpOn = maxMP;

	if (currentMovement == D_NONE && queuedDirs.size() > 0) {
		LivingEntity::handleDirectionInput(queuedDirs.front(), state);
		processedMoves++;
		queuedDirs.pop();
	}
	LivingEntity::update(state);
}

void Player::handleDirectionInput(Dir d, GameState *state) {
	if (currentMovement != D_NONE) {
		queuedDirs.push(d);
		if (queuedDirs.size() >= 3) {
			cout << "WARNING: Queued " << (int)queuedDirs.size() << " moves for player id " << (int)ID << endl;
		}
	}
	else {
		if (queuedDirs.size() > 0) {
			queuedDirs.push(d);
			d = queuedDirs.front();
			queuedDirs.pop();
		}
		LivingEntity::handleDirectionInput(d, state);
		processedMoves++;
	}
}

void Player::calculateCombatStats() {
	// damage, reloadTime, range, texture
	Stats overall = pInfo.getOverallStats();

	float mul = overall.getAtkMultiplier();
	def_t def = overall.getDef();

	ITEM item = pInfo.inv.equips[(int)Equipment::WEAPON];
	if (item != I_NONE && itemTypes[item]->location == Equipment::WEAPON) {
		Weapon *w = (Weapon*)itemTypes[item];
		combatStats = CombatStats((int)(mul * w->damage), w->reload, w->range, w->shot, overall.getSpeed(), def);
		numTargets = w->num_targets;
	}
	else {
		combatStats = noWeapon;
		combatStats.spd = pInfo.getOverallStats().getSpeed();
		numTargets = 0;
	}
	popTargets(numTargets);
}

void Player::fullHeal() {
	Stats st = pInfo.getOverallStats();
	
	maxLife = st.getHP();
	lifeOn = maxLife;

	maxMP = st.getMP();
	mpOn = maxMP;
}

const mp_t &Player::getMP() {
	return mpOn;
}


void Player::sendPacket(sf::Packet &packet) {
	for (int i = 0; i < 100; i++) {
		sf::Socket::Status status = socket->send(packet);

		if (status == sf::Socket::Done || status == sf::Socket::Disconnected) {
			dataSent += packet.getDataSize();
			break;
		}
	}

	//while (socket->send(packet) == sf::Socket::Status::Partial) {}
}

void Player::handleTarget(ID_t id) {
	LivingEntity::handleTarget(id);
	popTargets(numTargets);
}

#endif
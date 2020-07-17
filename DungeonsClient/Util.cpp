#include "pch.h"
#include "Util.h"
#include <iostream>
#include <random>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iomanip>
#include <sstream>
#include "CSMode.h"
#ifdef CLIENT
#include "Controller.h"
#include "State.h"
#endif

const float VIEW_HEIGHT = 1080.0f;
// NORMALLY 85
const int PIXEL_PER_UNIT = 85; // 20 good for map debugging, 30fps. 30 good for debugging too, 50fps.
const float PI = (float)M_PI;

pi dirOffset[5] = { pi(0, -1), pi(1, 0), pi(0, 1), pi(-1, 0), pi(0, 0) };
sf::Keyboard::Key dirKeys[4] = { sf::Keyboard::W, sf::Keyboard::D, sf::Keyboard::S, sf::Keyboard::A };

std::ostream &operator<<(std::ostream &os, const pi &p) {
	return os << "(" << p.x << ", " << p.y << ")";
}

Dir getCurrentDir() {
#ifdef CLIENT
	if (!windowFocus) return D_NONE;
	if (isTyping) return D_NONE;
#endif
	for (int i = 0; i < 4; i++) {
		if (sf::Keyboard::isKeyPressed(dirKeys[i])) return (Dir)i;
	}
	return D_NONE;
}

bool piComp::operator()(const pi &a, const pi &b) const {
	if (a.x == b.x) return a.y < b.y;
	return a.x < b.x;
}


namespace util {
	float rand(float a, float b)
	{
		static std::mt19937 gen;
		static bool set = false;
		static std::uniform_real_distribution<float> dis(0, 1);
		if (!set) {
			set = true;
			gen = std::mt19937((unsigned int)time(NULL));
		}
		return a + (b - a)*dis(gen);
	}

	// Inclusive
	int randint(int a, int b) {
		if (b < 0) return -randint(-b, -a);
		return (int)rand((float)a, (float)b + 0.999999f);
	}

	sf::Color colorLerp(sf::Color a, sf::Color b, float t) {
		return sf::Color(a.r*(1 - t) + b.r*t, a.g*(1 - t) + b.g*t, a.b*(1 - t) + b.b*t);
	}

	std::string floatToString(float a, int decimal_places) {
		std::stringstream stream;
		stream << std::fixed << std::setprecision(decimal_places) << a;
		return stream.str();
	}

	Dir getDirectionTowards(pi a, pi b) {
		if (b.y > a.y) return D_DOWN;
		if (b.y < a.y) return D_UP;
		if (b.x > a.x) return D_RIGHT;
		if (b.x < a.x) return D_LEFT;
		return D_NONE;
	}

	float mag(const sf::Vector2f &a) {
		return sqrt(a.x*a.x + a.y*a.y);
	}

	float dist(const sf::Vector2f &a, const sf::Vector2f &b) {
		return mag(a - b);
	}

	float ang(const sf::Vector2f &a) {
		return atan2(a.y, a.x);
	}

	float dot(const sf::Vector2f &a, const sf::Vector2f &b) {
		return a.x*b.x + a.y*b.y;
	}

	float crossMag(const sf::Vector2f &a, const sf::Vector2f &b) {
		return a.x * b.y - a.y * b.x;
	}

	int taxicab(pi a, pi b) {
		return abs(a.x - b.x) + abs(a.y - b.y);
	}

	// silly method but aSize is so small (like (5, 5) MAX almost always (1, 1) or (2, 2)) i cant be bothered to write more
	int taxicabSize(pi a, pi b, pi aSize) {
		int t = 1000000;
		for (int i = 0; i < aSize.x; i++) {
			for (int j = 0; j < aSize.y; j++) {
				t = std::min(t, taxicab(a + pi(i, j), b));
			}
		}
		return t;
	}

	void centerSfText(sf::Text &text) {
		auto textRect = text.getLocalBounds();
		text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	}

	pi randomBoxBorderPoint(int x1, int x2, int y1, int y2) {
		if (rand(0, 1) < 0.5f) {
			int y = (rand(0, 1) < 0.5f ? y1 : y2);
			return pi(rand(x1, x2), y);
		}
		int x = (rand(0, 1) < 0.5f ? x1 : x2);
		return pi(x, rand(y1, y2));
	}

	pi randomBoxPoint(int x1, int x2, int y1, int y2) {
		return pi(util::randint(x1, x2), util::randint(y1, y2));
	}

	std::vector<pi> borderPoints(pi center, pi tileSize, int width1, int width2) {
		std::vector<pi> vec;
		vec.reserve(100);

		int x, y, x1, y1, x2, y2;

		for (int width = width1; width <= width2; width++) {
			if (width < 0) continue;

			if (width == 0) {
				vec.push_back(center);
				continue;
			}

			x1 = center.x - width;
			x2 = center.x + tileSize.x + width - 1;

			y1 = center.y - width;
			y2 = center.y + tileSize.y + width - 1;

			for (x = x1; x <= x2; x++) {
				vec.push_back(pi(x, y1));
				vec.push_back(pi(x, y2));
			}
			for (y = y1 + 1; y <= y2 - 1; y++) {
				vec.push_back(pi(x1, y));
				vec.push_back(pi(x2, y));
			}
		}

		return vec;
	}

	int randomRound(float x) {
		if (x < 0) return -randomRound(-x);

		int below = (int)x;
		float t = x - below;

		// if t = 0, it must be below
		// if t = 0.1, it should be below 90%
		// if t = 0.6, it should be above with 60% chance

		if (util::rand(0, 1) <= t) return below + 1;
		return below;
	}

	int round(float x) {
		if (x < 0) return -round(-x);

		int below = (int)x;
		float t = x - below;

		return (t < 0.5f ? below : below + 1);
	}

	msgpair getCommand(std::string msg) {
		if (msg.size() == 0 || msg[0] != '/') {
			return msgpair("", "");
		}

		// command length
		int ln = msg.size() - 1;

		for (int i = 1; i < msg.size(); i++) {
			if (msg[i] == ' ') {
				ln = i-1;
				break;
			}
		}

		return msgpair(msg.substr(1, ln), (ln==msg.size()-1?"":msg.substr(ln + 1)));
	}

	pi getNextNum(const std::string &st, int start) {
		std::string numS = "";
		int i = start;
		for (; i < st.size(); i++) {
			char c = st[i];
			if ((c >= '0' && c <= '9') || c == '-') {
				numS.push_back(c);
			}
			else {
				break;
			}
		}

		if (numS.size() == 0) {
			cout << "util::getNextNum couldn't find a number" << endl;
			return pi(0, i);
		}
		return pi(std::stoi(numS), i);
	}
}








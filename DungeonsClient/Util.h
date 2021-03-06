#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include "Types.h"

using std::cout;
using std::endl;

extern const float PI;
extern const float VIEW_HEIGHT;
extern const int PIXEL_PER_UNIT;

template <typename T>
inline bool vectorContains(const std::vector<T> &vec, const T &t) {
	for (const T &i : vec) if (i == t) return true;
	return false;
}

std::ostream &operator<<(std::ostream &os, const pi &p);

enum Dir {
	D_UP, D_RIGHT, D_DOWN, D_LEFT, D_NONE
};
extern pi dirOffset[5];
extern sf::Keyboard::Key dirKeys[4];

struct piComp {
	bool operator()(const pi &a, const pi &b) const;
};

Dir getCurrentDir();

namespace util {
	float rand(float a, float b);
	int randint(int a, int b); //inclusive

	sf::Color colorLerp(sf::Color a, sf::Color b, float t);
	std::string floatToString(float a, int decimal_places = 2);

	Dir getDirectionTowards(pi a, pi b);

	float mag(const sf::Vector2f &a);
	float dist(const sf::Vector2f &a, const sf::Vector2f &b);
	float ang(const sf::Vector2f &a);
	float dot(const sf::Vector2f &a, const sf::Vector2f &b);
	float crossMag(const sf::Vector2f &a, const sf::Vector2f &b);

	int taxicab(pi a, pi b);
	int taxicabSize(pi a, pi b, pi aSize);

	void centerSfText(sf::Text &text);

	pi randomBoxBorderPoint(int x1, int x2, int y1, int y2); // x1<x2, y1<y2
	pi randomBoxPoint(int x1, int x2, int y1, int y2); // x1<x2, y1<y2

	std::vector<pi> borderPoints(pi center, pi tileSize, int width1 = 1, int width2 = 1);

	int randomRound(float x); // e.g. 1.1 has 90% chance to become 1
	int round(float x);

	typedef std::pair<std::string, std::string> msgpair;
	msgpair getCommand(std::string msg); // e.g. /setname james -> (setname, james)

	pi getNextNum(const std::string &st, int start=0); // returns (the number, the index where the string continues)

	//template <typename T> T randomFromWeightedVector(const std::vector<std::pair<T, float> > &vec);
	template <typename T> T randomFromWeightedVector(const std::vector<std::pair<T, float> > &vec) {
		float t = 0;
		for (int i = 0; i < (int)vec.size(); i++) {
			t += vec[i].second;
		}

		float x = util::rand(0, t);
		for (int i = 0; i < (int)vec.size(); i++) {
			if (vec[i].second >= x) return vec[i].first;
			x -= vec[i].second;
		}
		cout << "Failed to spawn from vector" << endl;
		return vec[vec.size() - 1].first;
	}
}
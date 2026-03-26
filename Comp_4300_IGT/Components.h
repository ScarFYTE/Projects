#pragma once
#include"Vec2.h"
#include <SFML/Graphics.hpp>

class CTransform {
public:
	Vec2 position = { 0.0,0.0 };
	Vec2 velocity = { 0.0,0.0 };
	float rotation = 0.0;

	CTransform() : position(0, 0), rotation(0), velocity(0.0, 0.0) {}
	CTransform(const Vec2& pos, const Vec2& vec, float rot) : position(pos), rotation(rot), velocity(vec) {}
};

class CCollision {
public:
	float radius;
	CCollision(float r) : radius(r) {}
};

class CScore {
public:
	int Score = 0;
	CScore() : Score(0) {}
	CScore(int s) : Score(s) {}
};

class CInput {
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;

	CInput() {}
};

class CShape {
	sf::CircleShape circle;

public:
	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float Thickness) :
		circle(radius, points)
	{
		circle.setFillColor(fill);
		circle.setOutlineColor(outline);
		circle.setOutlineThickness(Thickness);
		circle.setOrigin({ radius, radius });
	}
	void setPosition(const Vec2& pos) {
		circle.setPosition({ pos.x, pos.y });
	}
	void setRotation(float rot) {
		circle.setRotation(sf::degrees(rot));
	}

	float getRadius() const;
	sf::CircleShape getShape() const {
		return circle;
	}

	int getPointCount() const {
		return circle.getPointCount();
	}
};

class CLifeSpan {
public:
	int remaining = 0;
	int total = 0;
	CLifeSpan() : remaining(0), total(0) {}
	CLifeSpan(int Total) : remaining(Total), total(Total) {}
};
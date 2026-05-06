#pragma once
#include"Vec2.h"
#include <SFML/Graphics.hpp>
#include <string>

class CTransform {
public:
	Vec2 position = { 0.0,0.0 };
	Vec2 velocity = { 0.0,0.0 };
	float rotation = 0.0;
	bool onGround = false;

	CTransform() : position(0, 0), rotation(0), velocity(0.0, 0.0), onGround(false) {}
	CTransform(const Vec2& pos, const Vec2& vec, float rot) : position(pos), rotation(rot), velocity(vec), onGround(false) {}
};

// Legacy radius-based collision (kept for reference)
class CCollision {
public:
	float radius;
	CCollision(float r) : radius(r) {}
};

// Rectangular bounding box collision
class CBoundingBox {
public:
	Vec2 size; // full width and height
	CBoundingBox(float w, float h) : size(w, h) {}
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
	bool special = false;

	CInput() {}
};

// Legacy circle-based shape (kept for reference)
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
	void setFillColor(const sf::Color& color) {
		circle.setFillColor(color);
	}
	float getRadius() const;
	sf::CircleShape getShape() const {
		return circle;
	}

	int getPointCount() const {
		return circle.getPointCount();
	}
};

// Sprite/texture-based visual component (uses RectangleShape; swap fill for texture later)
class CSprite {
	sf::RectangleShape rect;
	// Heap-allocated so the address stays stable if CSprite is moved/copied
	std::unique_ptr<sf::Texture> texture;

public:
	CSprite(float w, float h, const sf::Color& fill) {
		rect.setSize({ w, h });
		rect.setFillColor(fill);
		rect.setOrigin({ w / 2.0f, h / 2.0f });
	}

	// Load a texture file; if successful the rectangle will display it instead of the fill color.
	bool loadTexture(const std::string& path) {
		auto tex = std::make_unique<sf::Texture>();
		if (tex->loadFromFile(path)) {
			texture = std::move(tex);
			rect.setTexture(texture.get());
			return true;
		}
		return false;
	}

	void setPosition(const Vec2& pos) {
		rect.setPosition({ pos.x, pos.y });
	}

	void setFillColor(const sf::Color& color) {
		rect.setFillColor(color);
	}

	Vec2 getSize() const {
		return { rect.getSize().x, rect.getSize().y };
	}

	sf::RectangleShape& getShape() {
		return rect;
	}
};

class CLifeSpan {
public:
	int remaining = 0;
	int total = 0;
	CLifeSpan() : remaining(0), total(0) {}
	CLifeSpan(int Total) : remaining(Total), total(Total) {}
};
#pragma once
#include "Vec2.h"
#include <SFML/Graphics.hpp>
#include <string>

class CParticle {
public:
float lifetime;
float age = 0.0f;
sf::Color color;

CParticle(float lifetime, sf::Color color)
: lifetime(lifetime), color(color) {
}

float alpha() const { return 1.0f - (age / lifetime); }
};

class CSprite {
sf::RectangleShape rect;
sf::Texture texture;

public:
CSprite(float w, float h, const sf::Color& color) : rect({ w, h }) {
rect.setFillColor(color);
rect.setOrigin({ w * 0.5f, h * 0.5f });
}

bool loadTexture(const std::string& path, bool repeat = false) {
if (!texture.loadFromFile(path)) { return false; }

if (repeat) {
texture.setRepeated(true);
rect.setTextureRect(sf::IntRect(
{ 0, 0 },
{ static_cast<int>(rect.getSize().x), static_cast<int>(rect.getSize().y) }
));
}

rect.setTexture(&texture);
return true;
}

void setPosition(const Vec2& pos) {
rect.setPosition({ pos.x, pos.y });
}

sf::RectangleShape& getShape() { return rect; }
const sf::RectangleShape& getShape() const { return rect; }
};

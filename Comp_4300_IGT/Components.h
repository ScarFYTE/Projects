#pragma once
#include"Vec2.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class CTransform {
public:
	Vec2  position  = { 0.0f, 0.0f };
	Vec2  velocity  = { 0.0f, 0.0f };
	float rotation  = 0.0f;
	bool  onGround  = false;

	int coyoteFrames = 0; // Frames left to allow jump after leaving ground
	int JumpBufferFrames = 0; // Frames left to allow jump after leaving ground

	CTransform() : position(0, 0), rotation(0), velocity(0.0f, 0.0f) {}
	CTransform(const Vec2& pos, const Vec2& vel, float rot)
		: position(pos), velocity(vel), rotation(rot) {}
};

// Axis-aligned bounding box (half-extents)
class CBoundingBox {
public:
	Vec2 halfSize;
	CBoundingBox(float w, float h) : halfSize(w * 0.5f, h * 0.5f) {}
	float width()  const { return halfSize.x * 2.0f; }
	float height() const { return halfSize.y * 2.0f; }

	CBoundingBox() : halfSize(0, 0) {}
	CBoundingBox(const Vec2& halfSize) : halfSize(halfSize) {}
};

class CParticle {
public:
	float lifetime;    // total lifespan in frames
	float age = 0.0f;  // how old it is
	sf::Color color;

	CParticle(float lifetime, sf::Color color)
		: lifetime(lifetime), color(color) {
	}

	float alpha() const { return 1.0f - (age / lifetime); } // Fade
};

// Sprite / visual rectangle (supports optional texture)
class CSprite {
	sf::RectangleShape rect;
	sf::Texture        texture;

public:
	CSprite(float w, float h, const sf::Color& color) : rect({ w, h }) {
		rect.setFillColor(color);
		rect.setOrigin({ w * 0.5f, h * 0.5f });
	}

	bool loadTexture(const std::string& path) {
		if (!texture.loadFromFile(path)) { return false; }
		rect.setTexture(&texture);
		return true;
	}

	void setPosition(const Vec2& pos) {
		rect.setPosition({ pos.x, pos.y });
	}

	sf::RectangleShape&       getShape()       { return rect; }
	const sf::RectangleShape& getShape() const { return rect; }
};

class CInput {
public:
	bool left    = false;
	bool right   = false;
	bool jump	 = false;
	CInput() {}
};

// Simplified health — lives removed; only spawn anchor and brief dead flag used during reset flash
class CHealth {
public:
	bool  isDead = false;
	Vec2  spawnPoint;

	CHealth() : spawnPoint(0, 0) {}
	CHealth(Vec2 spawnPoint) : spawnPoint(spawnPoint) {}
};

// Enemy patrol between waypoints
class CPatrol {
public:
	std::vector<Vec2> waypoints;
	int   currentTarget = 0;
	float speed         = 1.5f;
	bool  facingRight   = true;
};

// Enemy detection cone
class CSight {
public:
	float range        = 180.0f;
	float halfAngleDeg = 35.0f;
};

// Button / pressure plate that triggers another entity by tag
class CInteractable {
public:
	std::string linkedTag;
	bool        isPressed    = false;
	bool        requiresStay = true; // true = pressure plate, false = one-shot lever
};

// Sliding door that moves between open and closed positions
class CDoor {
public:
	bool  isOpen    = false;
	Vec2  openPos;
	Vec2  closedPos;
	float speed     = 3.0f;
	Vec2  savedHalfSize; // stored so the bounding box can be restored when closed
};

// Platform that moves between two positions when triggered
class CMovingPlatform {
public:
	Vec2  posA;
	Vec2  posB;
	bool  triggered = false;
	float speed     = 2.0f;
};

// Checkpoint that stores spawn positions for both players
class CCheckpoint {
public:
	bool activated = false;
	Vec2 p1Spawn;
	Vec2 p2Spawn;
};

// Exit zone — win when both players are inside
class CExit {
public:
	int playersInside = 0;
};
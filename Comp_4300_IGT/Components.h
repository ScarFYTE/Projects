#pragma once
#include"Vec2.h"
#include <SFML/Graphics.hpp>

class CTransform {
public:
	Vec2  position  = { 0.0f, 0.0f };
	Vec2  velocity  = { 0.0f, 0.0f };
	float rotation  = 0.0f;
	bool  onGround  = false;

	int coyoteFrames = 0; 
	int JumpBufferFrames = 0;

	CTransform() : position(0, 0), rotation(0), velocity(0.0f, 0.0f) {}
	CTransform(const Vec2& pos, const Vec2& vel, float rot)
		: position(pos), velocity(vel), rotation(rot) {}
};

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
	sf::Texture        texture;

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

	sf::RectangleShape&       getShape()       { return rect; }
	const sf::RectangleShape& getShape() const { return rect; }
};

class CInput {
public:
	bool left    = false;
	bool right   = false;
	bool jump	 = false;
	bool interact = false;	
	CInput() {}
};

class CHealth {
public:
	int   maxLives;
	int   lives;
	bool  isDead = false;
	int   respawnTimer = 0;    
	Vec2  spawnPoint;          

	CHealth(int lives, Vec2 spawnPoint)
		: maxLives(lives), lives(lives), spawnPoint(spawnPoint) {
	}
	CHealth() :maxLives(5), lives(5), spawnPoint(250, 250) {}

	bool isRespawning() const { return isDead && respawnTimer > 0; }
};

class CInteractable {
public:
	std::string linkedTag;       
	bool isPressed = false;
	bool requiresStay = true; 
	bool requiresInput = false;
};

class CDoor {
public:
	std::string linkTag;
	bool isOpen = false;

	CDoor() {}
};

class CMovingPlatform {
public:
	Vec2 posA;
	Vec2 posB;
	bool triggered = false;
	float speed = 2.0f;

	int requiredTriggers = 1;
	int currentTriggers = 0;
};

class CCheckpoint {
public:
	bool activated = false;
	Vec2 p1Spawn;
	Vec2 p2Spawn;
};

class CExit {
public:
	int playersInside = 0;
};


class CPatrol {
public:
	std::vector<Vec2> waypoints;
	int   currentTarget = 0;
	float speed = 1.5f;
	bool  facingRight = true;
};

class CSight {
public:
	float range = 100.0f;
	float halfAngleDeg = 35.0f;
};
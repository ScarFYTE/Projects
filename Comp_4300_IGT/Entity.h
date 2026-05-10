#pragma once
#include "Components.h"
#include <memory>
#include <string>
#include "Vec2.h"

class Entity {
	friend class EntityManager;

	bool        active = true;
	size_t      id     = 0;
	std::string tag    = "Default";
	Vec2 BoundingBoxcopy;

	Entity(const size_t id, const std::string& tag) : id(id), tag(tag) {}

public:
	std::shared_ptr<CTransform>   transform;
	std::shared_ptr<CBoundingBox> boundingBox;
	std::shared_ptr<CSprite>      sprite;
	std::shared_ptr<CInput>       input;
	std::shared_ptr<CParticle>    particle;
	std::shared_ptr<CHealth>	  health;
	std::shared_ptr<CMovingPlatform> movingPlatform;
	

	void 		SaveBoundingbox();
	Vec2		GetBoundingBoxCopy() const;
	void        Destroy();
	bool        IsActive() const;
	std::string& GetTag() const;
	size_t      GetID() const;
};
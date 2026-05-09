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

	Entity(const size_t id, const std::string& tag) : id(id), tag(tag) {}

public:
	std::shared_ptr<CTransform>      transform;
	std::shared_ptr<CBoundingBox>    boundingBox;
	std::shared_ptr<CSprite>         sprite;
	std::shared_ptr<CInput>          input;
	std::shared_ptr<CParticle>       particle;
	std::shared_ptr<CHealth>         health;
	std::shared_ptr<CPatrol>         patrol;
	std::shared_ptr<CSight>          sight;
	std::shared_ptr<CInteractable>   interactable;
	std::shared_ptr<CDoor>           door;
	std::shared_ptr<CMovingPlatform> movingPlatform;
	std::shared_ptr<CCheckpoint>     checkpoint;
	std::shared_ptr<CExit>           exit_;

	void        Destroy();
	bool        IsActive() const;
	std::string& GetTag() const;
	size_t      GetID() const;
};
#pragma once
#include "Components.h"
#include <memory>
#include <string>



class Entity {
	friend class EntityManager;
	
	
	bool active = true;
	size_t id = 0;
	std::string tag = "Default";

	Entity(const size_t id, const std::string& tag) : id(id), tag(tag) {}

public:

	std::shared_ptr<CTransform> transform;
	std::shared_ptr<CCollision> collision;
	std::shared_ptr<CScore> score;
	std::shared_ptr<CInput> input;
	std::shared_ptr<CShape> shape;
	std::shared_ptr<CLifeSpan> lifespan;

	void Destroy();
	bool IsActive() const;
	std::string& GetTag() const;
	size_t GetID() const;

};